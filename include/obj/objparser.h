#ifndef _OBJ_OBJPARSER_H_
#define _OBJ_OBJPARSER_H_

#include <obj/types.h>

namespace obj
{
	/*
	 *	OBJ File format description:
	 *	http://local.wasp.uwa.edu.au/~pbourke/dataformats/obj/
	 *	
	 *	Known issues:
	 *		. w coordinate in vectors not supported
	 *		. only reads first word from group name
	 *		. only reads first word from material name
	 *		. multiple material libraries not supported
	 */
	class objparser
	{
	public:
		objparser();

		void parse( const char* filename );
		void parse( std::istream& file );

		/************************************************************************/
		/* Parsing flags                                                        */
		/************************************************************************/

		bool convertNegativeIndices; // default = true

		/************************************************************************/
		/* Parsing notifications                                                */
		/* <lineNumber, message>                                                */
		/************************************************************************/

		// Error signal
		sig::signal2<unsigned int, const std::string&> errorSignal;

		// Comment signal
		sig::signal2<unsigned int, const std::string&> commentSignal;

		/************************************************************************/
		/* Individual geometry attributes                                       */
		/************************************************************************/

		// Vertex
		sig::signal1<const vec3d&> vertexSignal;

		// Normal
		sig::signal1<const vec3d&> normalSignal;
		
		// Texture coordinate (default value is zero)
		sig::signal1<const vec3d&> texcoordSignal;

		/************************************************************************/
		/* Face indices                                                         */
		/*		if index is zero, attribute is not defined in file              */
		/************************************************************************/

		// Start new primitive with given number of elements
		sig::signal1<unsigned int> faceBeginSignal;

		// Add nth element to current primitive
		sig::signal1<const face_index&> faceElementSignal;

		// End current primitive
		sig::signal0<> faceEndSignal;

		/************************************************************************/
		/* Object description                                                   */
		/************************************************************************/

		sig::signal1<const std::string&> objectNameSignal;

		sig::signal1<const std::string&> groupNameSignal;

		/************************************************************************/
		/* Material information                                                 */
		/************************************************************************/

		// Include material 'filename'
		sig::signal1<const std::string&> materialLibSignal;

		// Use material 'name' for next primitives
		sig::signal1<const std::string&> materialUseSignal;

	private:
		unsigned int _lineNumber;
		int _numVertices;
		int _numNormals;
		int _numTexCoords;

		void convertNegativeIndex( face_index& idx );
		bool parseIndexTuple( face_index& idx, const std::string& elem );
	};
}

#endif // _OBJ_OBJPARSER_H_
