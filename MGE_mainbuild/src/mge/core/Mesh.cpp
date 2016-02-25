#include "mge/core/Mesh.hpp"
#include <iostream>
#include <map>
#include <string>
#include <fstream>

using namespace std;
#include "mge/core/Renderer.hpp"
#include "mge/core/World.hpp"
#include "mge/core/Camera.hpp"
#include "glm.hpp"

std::map<std::string,Mesh*> Mesh::_meshCache;

Mesh::Mesh(string pId)
:	_id(pId), _indexBufferId(0), _vertexBufferId(0), _normalBufferId(0), _uvBufferId(0), _tangentBufferId(0)
{
	//ctor
}


Mesh::~Mesh()
{
	//dtor
}

Mesh* Mesh::load(string pFileName)
{
    Mesh* ret;
    std::map<std::string, Mesh*>::iterator pos = _meshCache.find(pFileName);

    if(pos==_meshCache.end())
    {
        ret = cache(pFileName);
        if(!ret)
        {
            throw "Mesh loading error. Aborting.";
        }
        _meshCache.insert(_meshCache.end(),std::pair<std::string,Mesh*>(pFileName,ret));

    }
    else
    {
        ret = _meshCache[pFileName];
        std::cout<<"Returning cached mesh "<< pFileName<<".\n";
    }
    return ret;
}

/**
 * Load reads the obj data into a new mesh using C++ combined with c style coding.
 * The result is an indexed mesh for use with glDrawElements.
 * Expects a obj file with following layout v/vt/vn/f eg
 *
 * For example the obj file for a simple plane describes two triangles, based on
 * four vertices, with 4 uv's all having the same vertex normals (NOT FACE NORMALS!)
 *
 * v 10.000000 0.000000 10.000000              //vertex 1
 * v -10.000000 0.000000 10.000000             //vertex 2
 * v 10.000000 0.000000 -10.000000             //vertex 3
 * v -10.000000 0.000000 -10.000000            //vertex 4
 * vt 0.000000 0.000000                        //uv 1
 * vt 1.000000 0.000000                        //uv 2
 * vt 1.000000 1.000000                        //uv 3
 * vt 0.000000 1.000000                        //uv 4
 * vn 0.000000 1.000000 -0.000000              //normal 1 (normal for each vertex is same)
 * s off
 *
 * Using these vertices, uvs and normals we can construct faces, made up of 3 triplets (vertex, uv, normal)
 * f 2/1/1 1/2/1 3/3/1                         //face 1 (triangle 1)
 * f 4/4/1 2/1/1 3/3/1                         //face 2 (triangle 2)
 *
 * So although this is a good format for blender and other tools reading .obj files, this is
 * not an index mechanism that OpenGL supports out of the box.
 * The reason is that OpenGL supports only one indexbuffer, and the value at a certain point in the indexbuffer, eg 3
 * refers to all three other buffers (v, vt, vn) at once,
 * eg if index[0] = 5, opengl will stream vertexBuffer[5], uvBuffer[5], normalBuffer[5] into the shader.
 *
 * So what we have to do after reading the file with all vertices, is construct unique indexes for
 * all pairs that are described by the faces in the object file, eg if you have
 * f 2/1/1 1/2/1 3/3/1                         //face 1 (triangle 1)
 * f 4/4/1 2/1/1 3/3/1                         //face 2 (triangle 2)
 *
 * v/vt/vn[0] will represent 2/1/1
 * v/vt/vn[1] will represent 1/2/1
 * v/vt/vn[2] will represent 3/3/1
 * v/vt/vn[3] will represent 4/4/1
 *
 * and that are all unique pairs, after which our index buffer can contain:
 *
 * 0,1,2,3,0,2
 *
 * So the basic process is, read ALL data into separate arrays, then use the faces to
 * create unique entries in a new set of arrays and create the indexbuffer to go along with it.
 *
 * Note that loading this mesh isn't cached like we do with texturing, this is an exercise left for the students.
 */
Mesh* Mesh::cache(string pFileName)
{
    cout << "Loading " << pFileName << "...";

	Mesh* mesh = new Mesh(pFileName);

	ifstream file (pFileName, ios::in);

	if( file.is_open() ){
        //these three vectors will contains data as taken from the obj file
        //in the order it is encountered in the object file
		vector<glm::vec3> vertices;
		vector<glm::vec3> normals;
		vector<glm::vec2> uvs;

        //in addition we create a map to store the triplets found under the f(aces) section in the
        //object file and map them to an index for our index buffer (just number them sequentially
        //as we encounter them and store references to the pack
		map <FaceIndexTriplet, unsigned int> mappedTriplets;

		string line; // to store each line in
		while(getline(file,line)) {

            // c-type string to store cmd read from obj file (cmd is v, vt, vn, f)
			char cmd[10];
			cmd[0] = 0;

            //get the first string in the line of max 10 chars (c-style)
			sscanf ( line.c_str(), "%10s", cmd );

			//note that although the if statements below seem to imply that we can
			//read these different line types (eg vertex, normal, uv) in any order,
			//this is just convenience coding for us (instead of multiple while loops)
			//we assume the obj file to list ALL v lines first, then ALL vt lines,
			//then ALL vn lines and last but not least ALL f lines last

			//so... start processing lines
			//are we reading a vertex line? straightforward copy into local vertices vector
			if ( strcmp ( cmd, "v" ) == 0 ) {
				glm::vec3 vertex;
				sscanf(line.c_str(), "%10s %f %f %f ", cmd, &vertex.x, &vertex.y, &vertex.z );
				vertices.push_back( vertex );

            //or are we reading a normal line? straightforward copy into local normal vector
			} else if ( strcmp ( cmd, "vn" ) == 0 ) {
				glm::vec3 normal;
				sscanf(line.c_str(), "%10s %f %f %f ", cmd, &normal.x, &normal.y, &normal.z );
				normals.push_back( normal );

            //or are we reading a uv line? straightforward copy into local uv vector
			} else if ( strcmp ( cmd, "vt" ) == 0 ) {
				glm::vec2 uv;
				sscanf(line.c_str(), "%10s %f %f ", cmd, &uv.x, &uv.y );
				uvs.push_back( uv );

            //this is where it gets nasty. After having read all vertices, normals and uvs into
            //their own buffer
			} else if ( strcmp ( cmd, "f" ) == 0 ) {

			    //an f lines looks like
			    //f 2/1/1 1/2/1 3/3/1
			    //in other words
			    //f v1/u1/n1 v2/u2/n2 v3/u3/n3
			    //for each triplet like that we need to check whether we already encountered it
			    //and update our administration based on that
				glm::ivec3 vertexIndex;
				glm::ivec3 normalIndex;
				glm::ivec3 uvIndex;
			    int count = sscanf(line.c_str(), "%10s %d/%d/%d %d/%d/%d %d/%d/%d", cmd, &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2] );

                //Have we read exactly 10 elements?
				if ( count == 10 ) {

                    //process 3 triplets, one for each vertex (which is first element of the triplet)
					for ( int i = 0; i < 3; ++i ) {
					    //create key out of the triplet and check if we already encountered this before
						FaceIndexTriplet triplet (vertexIndex[i], uvIndex[i], normalIndex[i]);
						map<FaceIndexTriplet, unsigned int>::iterator found = mappedTriplets.find(triplet);

						//if iterator points at the end, we haven't found it
						if (found == mappedTriplets.end())
                        {
                            //so create a new index value, and map our triplet to it
							unsigned int index = mappedTriplets.size();
							mappedTriplets[ triplet ] = index;

							//now record this index
							mesh->_indices.push_back( index );
							//and store the corresponding vertex/normal/uv values into our own buffers
							//note the -1 is required since all values in the f triplets in the .obj file
							//are 1 based, but our vectors are 0 based
							mesh->_vertices.push_back( vertices[ vertexIndex[i]-1 ] );
							mesh->_normals.push_back( normals[ normalIndex[i]-1 ] );
							mesh->_uvs.push_back( uvs[ uvIndex[i]-1 ] );
						}
						else
                        {
                            //if the key was already present, get the index value for it
							unsigned int index = found->second;
                            //and update our index buffer with it
							mesh->_indices.push_back( index );
						}
					}
				} else {
				    //If we read a different amount, something is wrong
					cout << "Error reading obj, needing v,vn,vt" << endl;
					delete mesh;
					return NULL;
				}
			}
		}

		file.close();
		_calculateTangents(mesh);
		mesh->_buffer();
        mesh->triangleCount = (int)(mesh->_indices.size()/3.0f);
		cout << "Mesh loaded and buffered:" << mesh->triangleCount<< " triangles." << endl;
		return mesh;
	} else {
		cout << "Could not read " << pFileName << endl;
		delete mesh;
		return NULL;
	}
}

void Mesh::_calculateTangents(Mesh*mesh)
{
    //mesh->_tangents = mesh->_vertices; // copy vertices to tangents as placeholders to copy results in

    for ( unsigned int i = 0; i < mesh->_indices.size(); i+=3 )
    {
		// get the triangle
		glm::vec3 v0 = mesh->_vertices[ mesh->_indices[ i+0 ] ]; // get triangle points in vertex space
		glm::vec3 v1 = mesh->_vertices[ mesh->_indices[ i+1 ] ];
		glm::vec3 v2 = mesh->_vertices[ mesh->_indices[ i+2 ] ];
		glm::vec2 u0 = mesh->_uvs[ mesh->_indices[ i+0 ] ]; // get triangle points in uv space
		glm::vec2 u1 = mesh->_uvs[ mesh->_indices[ i+1 ] ];
		glm::vec2 u2 = mesh->_uvs[ mesh->_indices[ i+2 ] ];

		glm::vec3 dVA = v1 - v0; // first edge of triangle in vertex space
		glm::vec3 dVB = v2 - v0; // second edge of triangle

		glm::vec2 dUA = u1 - u0; // first edge of triangle in uv space
		glm::vec2 dUB = u2 - u0; // second edge of triangle

		float fragment = 1.f / ( dUA.x* dUB.y - dUB.x * dUA.y ); // common fragment for invers UV

		// calc tangent for triangle, states how the tex is oriented on triangle
		glm::vec3 tangent   = glm::normalize( fragment * glm::vec3(  dUB.y * dVA.x - dUA.y * dVB.x,
																	 dUB.y * dVA.y - dUA.y * dVB.y,
																	 dUB.y * dVA.z - dUA.y * dVB.z ) );

		mesh->_tangents.push_back(tangent); // every vertex needs its tangent
		mesh->_tangents.push_back(tangent); // every vertex needs its tangent
		mesh->_tangents.push_back(tangent); // every vertex needs its tangent
		//cout << tangent << endl;
	}

/*
    for (int i = 0; i < mesh->_vertices.size(); i++) {
        glm::vec3 v0 = mesh->_vertices[i];
        glm::vec3 v1 = mesh->_vertices[i+1];
        glm::vec3 v2 = mesh->_vertices[i+2];

        glm::vec3 edge1 = v1 - v0;
        glm::vec3 edge2 = v2 - v0;

        glm::vec2 deltaUV1 = mesh->_uvs[i+1] - mesh->_uvs[i];
        glm::vec2 deltaUV2 = mesh->_uvs[i+2] - mesh->_uvs[i];

        float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

        glm::vec3 tangent;
        tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
        tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
        tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
        tangent = glm::normalize(tangent);

        mesh->_tangents.push_back(tangent);
        std::cout << tangent << std::endl;

        //mesh->_tangents[i] += tangent;
        //mesh->_tangents[i+1] += tangent;
        //mesh->_tangents[i+2] += tangent;

    }
*/
/*
    long triangleCount = mesh->_indices.size();
    long vertexCount = mesh->_vertices.size();
    vector<glm::vec3> tan1;
    vector<glm::vec3> tan2;

    for (long a = 0; a < triangleCount; a+=3)
    {
        long i1 = mesh->_indices[a];
        long i2 = mesh->_indices[a+1];
        long i3 = mesh->_indices[a+2];

        glm::vec3& v1 = mesh->_vertices[i1];
        glm::vec3& v2 = mesh->_vertices[i2];
        glm::vec3& v3 = mesh->_vertices[i3];

        glm::vec2& w1 = mesh->_uvs[i1];
        glm::vec2& w2 = mesh->_uvs[i2];
        glm::vec2& w3 = mesh->_uvs[i3];

        float x1 = v2.x - v1.x;
        float x2 = v3.x - v1.x;
        float y1 = v2.y - v1.y;
        float y2 = v3.y - v1.y;
        float z1 = v2.z - v1.z;
        float z2 = v3.z - v1.z;

        float s1 = w2.x - w1.x;
        float s2 = w3.x - w1.x;
        float t1 = w2.y - w1.y;
        float t2 = w3.y - w1.y;

        float r = 1.0F / (s1 * t2 - s2 * t1);
        glm::vec3 sdir((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r, (t2 * z1 - t1 * z2) * r);
        glm::vec3 tdir((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r, (s1 * z2 - s2 * z1) * r);

        tan1.push_back(sdir);
        tan1.push_back(sdir);
        tan1.push_back(sdir);

        tan2.push_back(tdir);
        tan2.push_back(tdir);
        tan2.push_back(tdir);

        //tan1[i1] += sdir;
        //tan1[i2] += sdir;
        //tan1[i3] += sdir;

        //tan2[i1] += tdir;
        //tan2[i2] += tdir;
        //tan2[i3] += tdir;
    }

    for (long a = 0; a < vertexCount; a++)
    {
        glm::vec3& n = mesh->_normals[a];
        glm::vec3& t = tan1[a];

        // Gram-Schmidt orthogonalize
        glm::vec3 tangent = glm::normalize(t - n * glm::dot(n, t));
        mesh->_tangents.push_back(tangent);
        // Calculate handedness
        //tangent.w = (glm::dot(glm::cross(n, t), tan2[a]) < 0.0f) ? -1.0f : 1.0f;
        //mesh->_tangents.push_back(tangent);
        std::cout << tangent << std::endl;
    }
    */
}

void Mesh::_buffer()
{
    glGenBuffers(1, &_indexBufferId);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBufferId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indices.size()*sizeof(unsigned int), &_indices[0], GL_STATIC_DRAW);

    glGenBuffers(1, &_vertexBufferId);
    glBindBuffer( GL_ARRAY_BUFFER, _vertexBufferId);
    glBufferData( GL_ARRAY_BUFFER, _vertices.size()*sizeof(glm::vec3), &_vertices[0], GL_STATIC_DRAW );

    glGenBuffers(1, &_normalBufferId);
    glBindBuffer( GL_ARRAY_BUFFER, _normalBufferId );
    glBufferData( GL_ARRAY_BUFFER, _normals.size()*sizeof(glm::vec3), &_normals[0], GL_STATIC_DRAW );

    glGenBuffers(1, &_uvBufferId);
    glBindBuffer( GL_ARRAY_BUFFER, _uvBufferId );
    glBufferData( GL_ARRAY_BUFFER, _uvs.size()*sizeof(glm::vec2), &_uvs[0], GL_STATIC_DRAW );

    glGenBuffers(1, &_tangentBufferId);
    glBindBuffer( GL_ARRAY_BUFFER, _tangentBufferId );
    glBufferData( GL_ARRAY_BUFFER, _tangents.size()*sizeof(glm::vec3), &_tangents[0], GL_STATIC_DRAW );

    glBindBuffer( GL_ARRAY_BUFFER, 0 );
}

void Mesh::streamToOpenGL(GLint pVerticesAttrib, GLint pNormalsAttrib, GLint pUVsAttrib, GLint pTangentAttrib) {
    if (pVerticesAttrib != -1) {
        glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferId);
        glEnableVertexAttribArray(pVerticesAttrib);
        glVertexAttribPointer(pVerticesAttrib, 3, GL_FLOAT, GL_FALSE, 0, (void*)0 );
    }

    if (pNormalsAttrib != -1) {
        glBindBuffer( GL_ARRAY_BUFFER, _normalBufferId);
        glEnableVertexAttribArray(pNormalsAttrib);
        glVertexAttribPointer(pNormalsAttrib, 3, GL_FLOAT, GL_TRUE, 0, (void*)0 );
    }

    if (pUVsAttrib != -1) {
        glBindBuffer( GL_ARRAY_BUFFER, _uvBufferId);
        glEnableVertexAttribArray(pUVsAttrib);
        glVertexAttribPointer(pUVsAttrib, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
    }

    if (pTangentAttrib != -1) {
        glBindBuffer( GL_ARRAY_BUFFER, _tangentBufferId);
        glEnableVertexAttribArray(pTangentAttrib);
        glVertexAttribPointer(pTangentAttrib, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    }

	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, _indexBufferId );

	glDrawElements(GL_TRIANGLES, _indices.size(), GL_UNSIGNED_INT, (GLvoid*)0);

    //stats
	Renderer::debugInfo.drawCallCount++;
    Renderer::debugInfo.triangleCount+=triangleCount;
	// no current buffer, to avoid mishaps, very important for performance

	glBindBuffer(GL_ARRAY_BUFFER,0);

	//fix for serious performance issue
	if (pUVsAttrib != -1) glDisableVertexAttribArray(pUVsAttrib);
	if (pNormalsAttrib != -1) glDisableVertexAttribArray(pNormalsAttrib);
	if (pVerticesAttrib != -1) glDisableVertexAttribArray(pVerticesAttrib);
	if (pTangentAttrib != -1) glDisableVertexAttribArray(pTangentAttrib);
}

void Mesh::streamToOpenGL(GLint pVerticesAttrib, GLint pNormalsAttrib, GLint pUVsAttrib) {
    if (pVerticesAttrib != -1) {
        glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferId);
        glEnableVertexAttribArray(pVerticesAttrib);
        glVertexAttribPointer(pVerticesAttrib, 3, GL_FLOAT, GL_FALSE, 0, (void*)0 );
    }

    if (pNormalsAttrib != -1) {
        glBindBuffer( GL_ARRAY_BUFFER, _normalBufferId);
        glEnableVertexAttribArray(pNormalsAttrib);
        glVertexAttribPointer(pNormalsAttrib, 3, GL_FLOAT, GL_TRUE, 0, (void*)0 );
    }

    if (pUVsAttrib != -1) {
        glBindBuffer( GL_ARRAY_BUFFER, _uvBufferId);
        glEnableVertexAttribArray(pUVsAttrib);
        glVertexAttribPointer(pUVsAttrib, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
    }

	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, _indexBufferId );

	glDrawElements(GL_TRIANGLES, _indices.size(), GL_UNSIGNED_INT, (GLvoid*)0);
    //stats
	Renderer::debugInfo.drawCallCount++;
	Renderer::debugInfo.triangleCount+=triangleCount;
	// no current buffer, to avoid mishaps, very important for performance

	glBindBuffer(GL_ARRAY_BUFFER,0);

	//fix for serious performance issue
	if (pUVsAttrib != -1) glDisableVertexAttribArray(pUVsAttrib);
	if (pNormalsAttrib != -1) glDisableVertexAttribArray(pNormalsAttrib);
	if (pVerticesAttrib != -1) glDisableVertexAttribArray(pVerticesAttrib);
}

void Mesh::enableInOpenGL(GLint pVerticesAttrib, GLint pNormalsAttrib, GLint pUVsAttrib)
{
    if (pVerticesAttrib != -1) {
        glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferId);
        glEnableVertexAttribArray(pVerticesAttrib);
        glVertexAttribPointer(pVerticesAttrib, 3, GL_FLOAT, GL_FALSE, 0, (void*)0 );
    }

    if (pNormalsAttrib != -1) {
        glBindBuffer( GL_ARRAY_BUFFER, _normalBufferId);
        glEnableVertexAttribArray(pNormalsAttrib);
        glVertexAttribPointer(pNormalsAttrib, 3, GL_FLOAT, GL_TRUE, 0, (void*)0 );
    }

    if (pUVsAttrib != -1) {
        glBindBuffer( GL_ARRAY_BUFFER, _uvBufferId);
        glEnableVertexAttribArray(pUVsAttrib);
        glVertexAttribPointer(pUVsAttrib, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
    }

    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, _indexBufferId );
}

void Mesh::drawInOpenGL()
{
    glDrawElements(GL_TRIANGLES, _indices.size(), GL_UNSIGNED_INT, (GLvoid*)0);
    //stats
	Renderer::debugInfo.drawCallCount++;
	Renderer::debugInfo.triangleCount+=triangleCount;
}

void Mesh::disableInOpenGL(GLint pVerticesAttrib, GLint pNormalsAttrib, GLint pUVsAttrib)
{
    glBindBuffer(GL_ARRAY_BUFFER,0);

	//fix for serious performance issue
	if (pUVsAttrib != -1) glDisableVertexAttribArray(pUVsAttrib);
	if (pNormalsAttrib != -1) glDisableVertexAttribArray(pNormalsAttrib);
	if (pVerticesAttrib != -1) glDisableVertexAttribArray(pVerticesAttrib);
}

void Mesh::renderDebugInfo(glm::mat4& pModelMatrix, World* pWorld) {
    //demo of how to render some debug info using the good ol' direct rendering mode...
    glUseProgram(0);
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(glm::value_ptr(pWorld->getMainCamera()->getProjection()));
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(glm::value_ptr(glm::inverse(pWorld->getMainCamera()->getWorldTransform()) * pModelMatrix));

    glBegin(GL_LINES);
    //for each index draw the normal starting at the corresponding vertex
    for (size_t i=0; i<_indices.size(); i++){
        //draw normal for vertex
        if (true) {
            //now get normal end
            glm::vec3 normal = _normals[_indices[i]];
            glColor3fv(glm::value_ptr(normal));

            glm::vec3 normalStart = _vertices[_indices[i]];
            glVertex3fv(glm::value_ptr(normalStart));
            glm::vec3 normalEnd = normalStart + normal*0.2f;
            glVertex3fv(glm::value_ptr(normalEnd));
        }

//        //draw tangent for vertex (code for tangent calculation has been removed)
          //tangents are needed if you want to implement bump mapping
//        if (_tangentsCalculated) {
//            glColor3f(1,1,1);
//            //now get normal end
//            glm::vec3 tangent = glm::vec3(_tangents[_indices[i]]);
//            glm::vec3 tangentStart = _vertices[_indices[i]];
//            glVertex3fv(glm::value_ptr(tangentStart));
//            glm::vec3 tangentEnd = tangentStart + tangent*0.2f;
//            glVertex3fv(glm::value_ptr(tangentEnd));
//        }

    }
    glEnd();
}



