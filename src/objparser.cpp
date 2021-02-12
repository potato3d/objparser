#include <obj/objparser.h>
#include <fstream>
#include <sstream>
#include <vector>
#include <ctype.h>

using namespace obj;

objparser::objparser()
{
	convertNegativeIndices = true;
}

void objparser::parse( const char* filename )
{
	std::ifstream file( filename );
	if( !file )
	{
		errorSignal.send( 0, "Cannot open file '" + std::string( filename ) + "'." );
		return;
	}

	parse( file );
}

void objparser::parse( std::istream& file )
{
	std::string line;
	_lineNumber = 0;
	_numVertices = 0;
	_numNormals = 0;
	_numTexCoords = 0;

	while( std::getline( file, line ) )
	{
		std::stringstream ss( line );
		++_lineNumber;

		// Read until next whitespace
		ss.unsetf( std::ios_base::skipws );
		ss >> std::ws;

		// Check empty line
		if( ss.eof() )
			continue;

		// Check comment line
		if( ss.peek() == '#' )
		{
			commentSignal.send( _lineNumber, line.c_str() + 2 );
			continue;
		}

		// Check keyword
		std::string keyword;
		ss >> keyword;

		// Case vertex
		if( keyword == "v" )
		{
			vec3d v;
			ss >> std::ws >> v.x >> std::ws >> v.y >> std::ws >> v.z >> std::ws;

			if( ss.fail() )
			{
				errorSignal.send( _lineNumber, "Parse error reading vertex, skipping it." );
				continue;
			}

			if( !ss.eof() )
				errorSignal.send( _lineNumber, "Ignoring information beyond third vertex value." );

			vertexSignal.send( v );
			++_numVertices;
		}
		// Case normal
		else if( keyword == "vn" )
		{
			vec3d n;
			ss >> std::ws >> n.x >> std::ws >> n.y >> std::ws >> n.z >> std::ws;

			if( ss.fail() )
			{
				errorSignal.send( _lineNumber, "Parse error reading normal, skipping it." );
				continue;
			}

			if( !ss.eof() )
				errorSignal.send( _lineNumber, "Ignoring information beyond third normal value." );

			normalSignal.send( n );
			++_numNormals;
		}
		// Case texcoord
		else if( keyword == "vt" )
		{
			vec3d t;
			ss >> std::ws >> t.x >> std::ws;
			
			// Optional parameter
			if( !ss.eof() )
				ss >> t.y >> std::ws;

			// Optional parameter
			if( !ss.eof() )
				ss >> t.z >> std::ws;

			if( ss.fail() )
			{
				errorSignal.send( _lineNumber, "Parse error reading texture coordinate, skipping it." );
				continue;
			}

			if( !ss.eof() )
				errorSignal.send( _lineNumber, "Ignoring information beyond third texcoord value." );

			texcoordSignal.send( t );
			++_numTexCoords;
		}
		// Case face
		else if( keyword == "f" || keyword == "fo" )
		{
			std::vector<std::string> elements;

			// Read each element as a separate string
			while( !ss.eof() )
			{
				std::string elem;
				ss >> std::ws >> elem >> std::ws;
				elements.push_back( elem );
			}

			if( ss.fail() || elements.empty() )
			{
				errorSignal.send( _lineNumber, "Parse error reading face list, skipping it." );
				continue;
			}

			// Begin face
			faceBeginSignal.send( (unsigned int)elements.size() );

			for( unsigned int i	= 0; i < elements.size(); ++i )
			{
				face_index idx;

				// Parse indices from nth element
				bool ok = parseIndexTuple( idx, elements[i] );

				if( ok )
					faceElementSignal.send( idx );
			}

			// End face
			faceEndSignal.send();
		}
		// Case object name
		else if( keyword == "o" )
		{
			objectNameSignal.send( line.c_str() + 2 );
		}
		// Case group name
		else if( keyword == "g" )
		{
			groupNameSignal.send( line.c_str() + 2 );
		}
		// Case material filename
		else if( keyword == "mtllib" )
		{
			std::string filename;

			ss.setf( std::ios_base::skipws );

			while( !ss.eof() )
			{
				std::string s;
				ss >> s;
				filename += s + " ";
			}

			//ss >> std::ws >> filename >> std::ws;

			if( ss.fail() )
			{
				errorSignal.send( _lineNumber, "Parse error reading material library filename, skipping it." );
				continue;
			}

			if( !ss.eof() )
				errorSignal.send( _lineNumber, "Ignoring information beyond first material library filename." );

			materialLibSignal.send( filename );
		}
		// Case material use
		else if( keyword == "usemtl" )
		{
			std::string material;
			ss >> std::ws >> material >> std::ws;

			if( ss.fail() )
			{
				errorSignal.send( _lineNumber, "Parse error reading material name, skipping it." );
				continue;
			}

			if( !ss.eof() )
				errorSignal.send( _lineNumber, "Ignoring information beyond first material name." );

			materialUseSignal.send( material );
		}
		// Case unknown
		else
		{
			errorSignal.send( _lineNumber, "Unknown keyword '" + keyword + "', skipping line." );
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// Private
//////////////////////////////////////////////////////////////////////////
void objparser::convertNegativeIndex( face_index& idx )
{
	if( idx.vertexIdx < 0 )
		idx.vertexIdx += _numVertices + 1;

	if( idx.normalIdx < 0 )
		idx.normalIdx += _numNormals + 1;

	if( idx.texCoordIdx < 0 )
		idx.texCoordIdx += _numTexCoords + 1;
}

bool objparser::parseIndexTuple( face_index& idx, const std::string& elem )
{
	std::stringstream elemSS( elem );

	// Possible cases: v, v/t, v//n, v/t/n
	elemSS >> idx.vertexIdx;

	// Check for t and n indices
	if( !elemSS.eof() && elemSS.peek() == '/' )
	{
		char slash1;
		elemSS >> slash1;

		if( !elemSS.eof() && isdigit( elemSS.peek() ) )
		{
			// We have at least v/t
			elemSS >> idx.texCoordIdx;
		}

		// Case v//n or v/t/n
		if( !elemSS.eof() && elemSS.peek() == '/' )
		{
			char slash2;
			elemSS >> slash2 >> idx.normalIdx;
		}
	}

	// Check for errors
	if( elemSS.fail() || !elemSS.eof() )
	{
		errorSignal.send( _lineNumber, "Parse error reading face element, skipping it." );
		return false;
	}

	// Check if we need to convert negative indices
	if( convertNegativeIndices )
		convertNegativeIndex( idx );

	return true;
}
