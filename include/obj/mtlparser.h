#ifndef _OBJ_MTLPARSER_H_
#define _OBJ_MTLPARSER_H_

#include <obj/types.h>

namespace obj
{
	/*
	 *	MTL File format description:
	 *	http://local.wasp.uwa.edu.au/~pbourke/dataformats/mtl/
	 *	
	 *	Known issues:
	 *		. texture map options not supported
	 *		. only reads first word from texture filename
	 */
	class mtlparser
	{
	public:
		void parse( const char* filename );
		void parse( std::istream& file );

		/************************************************************************/
		/* Parsing notifications                                                */
		/* <lineNumber, message>                                                */
		/************************************************************************/

		// Error signal
		sig::signal2<unsigned int, const std::string&> errorSignal;

		// Comment signal
		sig::signal2<unsigned int, const std::string&> commentSignal;

		/************************************************************************/
		/* Material color and illumination                                      */
		/************************************************************************/

		// Begin new material definition
		sig::signal1<const std::string&> beginMaterialSignal;

		// Ambient color
		sig::signal1<const vec3d&> ambientSignal;

		// Diffuse color
		sig::signal1<const vec3d&> diffuseSignal;

		// Specular color
		sig::signal1<const vec3d&> specularSignal;

		// Specular exponent
		sig::signal1<double> specularExpSignal;

		// Opacity
		sig::signal1<double> opacitySignal;

		// Refraction index
		sig::signal1<double> refractionIndexSignal;

		/************************************************************************/
		/* Texture maps                                                         */
		/************************************************************************/

		// Ambient texture
		sig::signal1<const std::string&> textureAmbientSignal;

		// Diffuse texture
		sig::signal1<const std::string&> textureDiffuseSignal;

		// Specular
		sig::signal1<const std::string&> textureSpecularSignal;

	private:
		bool parseTextureMap( unsigned int lineNumber, std::stringstream& ss, std::string& filename );
	};
}

#endif // _OBJ_MTLPARSER_H_
