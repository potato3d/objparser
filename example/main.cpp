#include <obj/objparser.h>
#include <obj/mtlparser.h>
#include <iostream>
#include <sstream>
#include <fstream>

class mtlparser_tester : public sig::has_slots<>
{
public:
	mtlparser_tester( const char* logfile )
	{
		file.open( logfile );
	}

	void connect( obj::mtlparser& parser )
	{
		// Connect signals to slots
		parser.errorSignal.connect( this, &mtlparser_tester::error_slot );
		parser.commentSignal.connect( this, &mtlparser_tester::comment_slot );
		parser.beginMaterialSignal.connect( this, &mtlparser_tester::beginMaterial_slot );
		parser.ambientSignal.connect( this, &mtlparser_tester::ambient_slot );
		parser.diffuseSignal.connect( this, &mtlparser_tester::diffuse_slot );
		parser.specularSignal.connect( this, &mtlparser_tester::specular_slot );
		parser.specularExpSignal.connect( this, &mtlparser_tester::specularExp_slot );
		parser.opacitySignal.connect( this, &mtlparser_tester::opacity_slot );
		parser.refractionIndexSignal.connect( this, &mtlparser_tester::refractionIndex_slot );
		parser.textureAmbientSignal.connect( this, &mtlparser_tester::textureAmbient_slot );
		parser.textureDiffuseSignal.connect( this, &mtlparser_tester::textureDiffuse_slot );
		parser.textureSpecularSignal.connect( this, &mtlparser_tester::textureSpecular_slot );
	}

	void error_slot( unsigned int lineNumber, const std::string& msg )
	{
		file << "error (" << lineNumber << "): " << msg << std::endl;
	}

	void comment_slot( unsigned int lineNumber, const std::string& msg )
	{
		file << "comment (" << lineNumber << "): " << msg << std::endl;
	}

	void  beginMaterial_slot( const std::string& name )
	{
		file << "new material: " << name << std::endl;
	}

	void ambient_slot( const obj::vec3d& color )
	{
		file << "  ambient  [" << color.x << ", " << color.y << ", " << color.z << "]" << std::endl;
	}

	void diffuse_slot( const obj::vec3d& color )
	{
		file << "  diffuse [" << color.x << ", " << color.y << ", " << color.z << "]" << std::endl;
	}

	void specular_slot( const obj::vec3d& color )
	{
		file << "  specular [" << color.x << ", " << color.y << ", " << color.z << "]" << std::endl;
	}

	void specularExp_slot( double value )
	{
		file << "  specular exp: " << value << std::endl;
	}

	void opacity_slot( double value )
	{
		file << "  opacity: " << value << std::endl;
	}

	void refractionIndex_slot( double value )
	{
		file << "  refraction index: " << value << std::endl;
	}

	void textureAmbient_slot( const std::string& filename )
	{
		file << "  texture ambient: " << filename << std::endl;
	}

	void textureDiffuse_slot( const std::string& filename )
	{
		file << "  texture diffuse: " << filename << std::endl;
	}

	void textureSpecular_slot( const std::string& filename )
	{
		file << "  texture specular: " << filename << std::endl;
	}

	std::ofstream file;
};

class objparser_tester : public sig::has_slots<>
{
public:
	objparser_tester( const char* logfile )
	{
		file.open( logfile );
	}

	void connect( obj::objparser& parser )
	{
		// Connect signals to slots
		parser.errorSignal.connect( this, &objparser_tester::error_slot );
		parser.commentSignal.connect( this, &objparser_tester::comment_slot );
		parser.vertexSignal.connect( this, &objparser_tester::vertex_slot );
		parser.normalSignal.connect( this, &objparser_tester::normal_slot );
		parser.texcoordSignal.connect( this, &objparser_tester::texcoord_slot );
		parser.faceBeginSignal.connect( this, &objparser_tester::polygonBegin_slot );
		parser.faceElementSignal.connect( this, &objparser_tester::polygonElement_slot );
		parser.faceEndSignal.connect( this, &objparser_tester::polygonEnd_slot );
		parser.objectNameSignal.connect( this, &objparser_tester::objectName_slot );
		parser.groupNameSignal.connect( this, &objparser_tester::groupName_slot );
		parser.materialLibSignal.connect( this, &objparser_tester::materialLib_slot );
		parser.materialUseSignal.connect( this, &objparser_tester::materialUse_slot );
	}

	void error_slot( unsigned int lineNumber, const std::string& msg )
	{
		file << "error (" << lineNumber << "): " << msg << std::endl;
	}

	void comment_slot( unsigned int lineNumber, const std::string& msg )
	{
		file << "comment (" << lineNumber << "): " << msg << std::endl;
	}

	void vertex_slot( const obj::vec3d& v )
	{
		file << "vertex [" << v.x << ", " << v.y << ", " << v.z << "]" << std::endl;
	}

	void normal_slot( const obj::vec3d& v )
	{
		file << "normal [" << v.x << ", " << v.y << ", " << v.z << "]" << std::endl;
	}

	void texcoord_slot( const obj::vec3d& v )
	{
		file << "texcoord [" << v.x << ", " << v.y << ", " << v.z << "]" << std::endl;
	}

	void polygonBegin_slot( unsigned int numElements )
	{
		file << "begin polygon (" << numElements << " elements)" << std::endl;
	}

	void polygonElement_slot( const obj::face_index& idx )
	{
		file << "  element " << std::endl;
		file << "    vertex   " << idx.vertexIdx << std::endl;
		file << "    texcoord " << idx.texCoordIdx << std::endl;
		file << "    normal   " << idx.normalIdx << std::endl;
	}

	void polygonEnd_slot()
	{
		file << "end polygon" << std::endl;
	}

	void objectName_slot( const std::string& name )
	{
		file << "object: " << name << std::endl;
	}

	void groupName_slot( const std::string& name )
	{
		file << "group: " << name << std::endl;
	}

	void materialLib_slot( const std::string& filename )
	{
		file << "use material file: " << filename << std::endl;

		// Check mtl with parser tester
		obj::mtlparser mtl_parser;
		mtlparser_tester tester( "mtl_parse_log.txt" );
		tester.connect( mtl_parser );

		mtl_parser.parse( filename.c_str() );
	}

	void materialUse_slot( const std::string& name )
	{
		file << "use material: " << name << std::endl;
	}

	std::ofstream file;
};

int main( int argc, char** argv )
{
	if( argc < 2 )
	{
		std::cout << "Please supply an OBJ file as command line argument." << std::endl;
		system( "pause" );
		return 0;
	}

	obj::objparser parser;

	objparser_tester tester( "obj_parse_log.txt" );
	tester.connect( parser );

	parser.parse( argv[1] );

	return 0;
}
