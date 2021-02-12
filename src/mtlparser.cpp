#include <obj/mtlparser.h>
#include <fstream>
#include <sstream>

using namespace obj;

void mtlparser::parse( const char* filename )
{
	std::ifstream file( filename );
	if( !file )
	{
		errorSignal.send( 0, "Cannot open file '" + std::string( filename ) + "'." );
		return;
	}

	parse( file );
}

void mtlparser::parse( std::istream& file )
{
	std::string line;
	unsigned int lineNumber = 0;

	while( std::getline( file, line ) )
	{
		std::stringstream ss( line );
		++lineNumber;

		// Read until next whitespace
		ss.unsetf( std::ios_base::skipws );
		ss >> std::ws;

		// Check empty line
		if( ss.eof() )
			continue;

		// Check comment line
		if( ss.peek() == '#' )
		{
			commentSignal.send( lineNumber, line.c_str() + 2 );
			continue;
		}

		// Check keyword
		std::string keyword;
		ss >> keyword;
		
		// Case new material
		if( keyword == "newmtl" )
		{
			std::string name;
			ss >> std::ws >> name >> std::ws;

			if( ss.fail() )
			{
				errorSignal.send( lineNumber, "Parse error reading material name, skipping it." );
				continue;
			}

			if( !ss.eof() )
				errorSignal.send( lineNumber, "Ignoring information beyond first material name." );

			beginMaterialSignal.send( name );
		}
		// Case ambient
		else if( keyword == "Ka" )
		{
			vec3d a;
			ss >> std::ws;

			// Check option
			if( !isdigit( ss.peek() ) )
			{
				errorSignal.send( lineNumber, "Ambient color not RGB, skipping it." );
				continue;
			}

			ss >> a.x >> std::ws >> a.y >> std::ws >> a.z >> std::ws;

			if( ss.fail() )
			{
				errorSignal.send( lineNumber, "Parse error reading ambient color, skipping it." );
				continue;
			}

			if( !ss.eof() )
				errorSignal.send( lineNumber, "Ignoring information beyond third ambient color value." );

			ambientSignal.send( a );
		}
		// Case diffuse
		else if( keyword == "Kd" )
		{
			vec3d d;
			ss >> std::ws;

			// Check option
			if( !isdigit( ss.peek() ) )
			{
				errorSignal.send( lineNumber, "Diffuse color not RGB, skipping it." );
				continue;
			}

			ss >> std::ws >> d.x >> std::ws >> d.y >> std::ws >> d.z >> std::ws;

			if( ss.fail() )
			{
				errorSignal.send( lineNumber, "Parse error reading diffuse color, skipping it." );
				continue;
			}

			if( !ss.eof() )
				errorSignal.send( lineNumber, "Ignoring information beyond third diffuse color value." );

			diffuseSignal.send( d );
		}
		// Case specular
		else if( keyword == "Ks" )
		{
			vec3d s;
			ss >> std::ws;

			// Check option
			if( !isdigit( ss.peek() ) )
			{
				errorSignal.send( lineNumber, "Specular color not RGB, skipping it." );
				continue;
			}

			ss >> std::ws >> s.x >> std::ws >> s.y >> std::ws >> s.z >> std::ws;

			if( ss.fail() )
			{
				errorSignal.send( lineNumber, "Parse error reading specular color, skipping it." );
				continue;
			}

			if( !ss.eof() )
				errorSignal.send( lineNumber, "Ignoring information beyond third specular color value." );

			specularSignal.send( s );
		}
		// Case dissolve factor (opacity)
		else if( keyword == "d" || keyword == "Tr" )
		{
			ss >> std::ws;

			// If any options, skip field
			if( ss.peek() == '-' )
			{
				errorSignal.send( lineNumber, "Opacity with options it not supported, skipping it." );
				return;
			}

			double e;
			ss >> std::ws >> e >> std::ws;

			if( ss.fail() )
			{
				errorSignal.send( lineNumber, "Parse error reading opacity, skipping it." );
				continue;
			}

			if( !ss.eof() )
				errorSignal.send( lineNumber, "Ignoring information beyond opacity value." );

			opacitySignal.send( e );
		}
		// Case specular exponent
		else if( keyword == "Ns" )
		{
			double e;
			ss >> std::ws >> e >> std::ws;

			if( ss.fail() )
			{
				errorSignal.send( lineNumber, "Parse error reading specular exponent, skipping it." );
				continue;
			}

			if( !ss.eof() )
				errorSignal.send( lineNumber, "Ignoring information beyond specular exponent value." );

			specularExpSignal.send( e );
		}
		// Case refraction index
		else if( keyword == "Ni" )
		{
			double i;
			ss >> std::ws >> i >> std::ws;

			if( ss.fail() )
			{
				errorSignal.send( lineNumber, "Parse error reading refraction index, skipping it." );
				continue;
			}

			if( !ss.eof() )
				errorSignal.send( lineNumber, "Ignoring information beyond refraction index value." );

			refractionIndexSignal.send( i );
		}
		// Case ambient texture map
		else if( keyword == "map_Ka" || keyword == "map_a")
		{
			std::string filename;
			bool ok = parseTextureMap( lineNumber, ss, filename );			
			if( ok )
				textureAmbientSignal.send( filename );
		}
		// Case diffuse texture map
		else if( keyword == "map_Kd" || keyword == "map_d" || keyword == "map_D" )
		{
			std::string filename;
			bool ok = parseTextureMap( lineNumber, ss, filename );			
			if( ok )
				textureDiffuseSignal.send( filename );
		}
		// Case specular texture map
		else if( keyword == "map_Ks" || keyword == "map_s" )
		{
			std::string filename;
			bool ok = parseTextureMap( lineNumber, ss, filename );			
			if( ok )
				textureSpecularSignal.send( filename );
		}
		// Case unknown
		else
		{
			errorSignal.send( lineNumber, "Unknown keyword '" + keyword + "', skipping line." );
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// Private
//////////////////////////////////////////////////////////////////////////
bool mtlparser::parseTextureMap( unsigned int lineNumber, std::stringstream& ss, std::string& filename )
{
	ss >> std::ws;

	if( ss.peek() == '-' )
		errorSignal.send( lineNumber, "Skipping texture map options." );

	while( !ss.eof() )
		ss >> std::ws >> filename;

	if( ss.fail() )
	{
		errorSignal.send( lineNumber, "Parse error reading texture map, skipping it." );
		return false;
	}

	return true;
}
