#include "3ds.h"
#include "stlpache.h"
using namespace stlpache;

// 构造函数
C3DSModel::C3DSModel()
{
	// 初始化文件指针
	m_FilePtr = NULL;

	// 定义一个默认的材质（灰色）
	tMaterial defaultMat;
	defaultMat.isTexMat = false;
	strcpy(defaultMat.matName.string, "5DG_Default");
	defaultMat.color[0] = 192;
	defaultMat.color[1] = 192;
	defaultMat.color[2] = 192;
	m_3DModel.pMaterials.push_back(defaultMat);

	// 初始化保存3DS模型的结构体
	m_3DModel.numOfMaterials = 1;
	m_3DModel.numOfObjects = 0;
}

// 析构函数
C3DSModel::~C3DSModel()
{
	m_3DModel.pMaterials.clear();
	m_3DModel.pObject.clear();
}

// 载入3ds文件
bool C3DSModel::Load(const char *strFileName)
{
	char strMessage[128] = {0};
	tChunk chunk = {0};

	// 打开文件
	m_FilePtr = fopen(strFileName,"rb");

	// 如果文件打开失败
	if (!m_FilePtr)
	{
		sprintf(strMessage, "3DS文件 %s 不存在！", strFileName);
		return false;
	}

	// 读取3ds文件的第一个Chunk
	ReadChunk(&chunk);

	// 检查是否是3ds文件
	if (chunk.ID != PRIMARY)
	{
		sprintf(strMessage, "读取文件 %s 失败！", strFileName);
		fclose(m_FilePtr);
		return false;
	}

	// 开始读取3ds文件
	ReadPrimary(chunk.length-6);

	// 计算每个顶点的法线量
	//yangxixi不用他的计算法线，效率不高，用我们自己的
	//ComputeNormals();

	// 关闭打开的文件
	fclose(m_FilePtr);
	m_FilePtr = NULL;

	return true;
}

// 从文件中读取1个字节
BYTE C3DSModel::ReadByte(void)
{
	BYTE result = 0;
	fread(&result, 1, 1, m_FilePtr);
	return result;
}

// 从文件中读取2个字节
WORD C3DSModel::ReadWord(void)
{
	return ReadByte() + (ReadByte()<<8);
}

// 从文件中读取4个字节
UINT C3DSModel::ReadUint(void)
{
	return ReadWord() + (ReadWord()<<16);
}

// 从文件中读取浮点数
float C3DSModel::ReadFloat(void)
{
	float result;
	fread(&result, sizeof(float), 1, m_FilePtr);
	return result;
}

// 从文件中读取字符串（返回字符串长度）
UINT C3DSModel::ReadString(STRING *pStr)
{
	int n=0;
	while ((pStr->string[n++]=ReadByte()) != 0)
		;
	return n;
}

// 读取3ds的一个Chunk信息
void C3DSModel::ReadChunk(tChunk *pChunk)
{
	fread(&pChunk->ID, 1, 2, m_FilePtr);
	fread(&pChunk->length, 1, 4, m_FilePtr);
}

// 读取3ds文件主要Chunk
UINT C3DSModel::ReadPrimary(UINT n)
{
	UINT count = 0;				// 该Chunk内容已读取的字节计数
	tChunk chunk = {0};			// 用以保存子Chunk的内容
	while (count < n)
	{
		ReadChunk(&chunk);
		switch (chunk.ID)
		{
		case PRIM_EDIT:
			ReadEdit(chunk.length-6);
			break;
		//case PRIM_KEY:
		//	ReadKeyframe(chunk.length-6);
		//	break;
		default:
			fseek(m_FilePtr, chunk.length-6, SEEK_CUR);
			break;
		}
		count += chunk.length;
	}
	return count;
}

// 读取3ds物体主编辑Chunk
UINT C3DSModel::ReadEdit(UINT n)
{
	UINT count = 0;
	tChunk chunk = {0};
	while(count < n)
	{
		ReadChunk(&chunk);
		switch(chunk.ID)
		{
		case EDIT_MAT:
			ReadMaterial(chunk.length-6);
			break;
		case EDIT_OBJECT:
			ReadObject(chunk.length-6);
			break;
		default:
			fseek(m_FilePtr, chunk.length-6, SEEK_CUR);
			break;
		}
		count += chunk.length;
	}
	return count;
}

// 读取3ds对象
UINT C3DSModel::ReadObject(UINT n)
{
	UINT count = 0;
	tChunk chunk = {0};
	// 新的3ds对象
	t3DObject newObject = {0};
	count += ReadString(&newObject.objName);
	m_3DModel.numOfObjects ++;

	while (count < n)
	{
		ReadChunk(&chunk);
		switch (chunk.ID)
		{
		case OBJECT_INFO:
			ReadObjectInfo(&newObject, n-count -6);
			break;
		default:
			fseek(m_FilePtr, chunk.length-6, SEEK_CUR);
			break;
		}
		count += chunk.length;
	}
	// 保存3ds对象
	m_3DModel.pObject.push_back(newObject);
	return count;
}

// 读取3ds对象信息
UINT C3DSModel::ReadObjectInfo(t3DObject *pObj, UINT n)
{
	UINT count = 0;
	tChunk chunk = {0};

	while (count < n)
	{
		ReadChunk(&chunk);
		switch (chunk.ID)
		{
		case OBJECT_VERTEX:
			pObj->numOfVerts = ReadWord();
			pObj->pVerts = new vector3<double>[pObj->numOfVerts];
			memset(pObj->pVerts, 0, sizeof(vector3<double>) * pObj->numOfVerts);
			// 按块读取顶点坐标值
			// 错误很可能产生在这里，用class后，不是连在一起储存！以后再改了，困了2012-04-01 00:34
			// fread(pObj->pVerts, 1, chunk.length - 8, m_FilePtr);
			// 仿照面的读取方法 调换y、z坐标值(由于3dMAX坐标系方向与OpenGL不同)2012-04-11 23:21
			for (int i=0; i<pObj->numOfVerts; i++)
			{
				pObj->pVerts[i].set_x(ReadFloat());
				pObj->pVerts[i].set_z(ReadFloat());
				pObj->pVerts[i].set_y(ReadFloat());
			}
			break;
		case OBJECT_FACET:
			ReadFacetInfo(pObj,chunk.length-6);
			break;
		case OBJECT_UV:
			pObj->numTexVertex = ReadWord();
			pObj->pTexVerts = new vector2f[pObj->numTexVertex];
			memset(pObj->pTexVerts, 0, sizeof(vector2f) * pObj->numTexVertex);
			// 按块读取纹理坐标值
			//fread(pObj->pTexVerts, 1, chunk.length - 8, m_FilePtr);
			for (int i = 0; i < pObj->numTexVertex; i++)
			{
				pObj->pTexVerts[i].set_x(ReadFloat());
				pObj->pTexVerts[i].set_y(ReadFloat());
			}
			break;
		default:
			fseek(m_FilePtr, chunk.length-6, SEEK_CUR);
			break;
		}
		count += chunk.length;
	}
	return count;
}

// 读取面信息
UINT C3DSModel::ReadFacetInfo(t3DObject *pObj, UINT n)
{
	UINT count = 0;
	tChunk chunk = {0};
	pObj->numOfFaces = ReadWord();
	pObj->pFaces = new tFace[pObj->numOfFaces];
	memset(pObj->pFaces, 0, sizeof(tFace) * pObj->numOfFaces);

	// 读取面索引值(第4个值为3dMAX使用的参数，舍弃)
	for (int i=0; i<pObj->numOfFaces; i++)
	{
		pObj->pFaces[i].vertIndex[0] = ReadWord();
		pObj->pFaces[i].vertIndex[1] = ReadWord();
		pObj->pFaces[i].vertIndex[2] = ReadWord();
		ReadWord();
	}
	count +=2+pObj->numOfFaces*8;

	STRING matName;
	int t;
	int matID = 0;
	while (count < n)
	{
		ReadChunk(&chunk);
		switch (chunk.ID)
		{
		case FACET_MAT:
			ReadString(&matName);			// 材质名称
			t=ReadWord();					// 材质对应的面个数
			// 查找对应的材质
			for (int i=1;i<=m_3DModel.numOfMaterials;i++)
			{
				if (strcmp(matName.string, m_3DModel.pMaterials[i].matName.string) == 0)
				{
					matID = i;
					break;
				}
			}
			// 依据面索引给每个面绑定材质ID
			while (t>0)
			{
				pObj->pFaces[ReadWord()].matID = matID;
				t--;
			}
			break;
		default:
			fseek(m_FilePtr, chunk.length-6, SEEK_CUR);
			break;
		}
		count += chunk.length;
	}
	return count;
}

// 读取材质
UINT C3DSModel::ReadMaterial(UINT n)
{
	UINT count = 0;
	tChunk chunk = {0};
	// 新的材质
	tMaterial newMaterial = {0};
	m_3DModel.numOfMaterials ++;
	while (count < n)
	{
		ReadChunk(&chunk);
		switch (chunk.ID)
		{
		case MAT_NAME:
			ReadString(&newMaterial.matName);
			break;
		case MAT_DIF:
			ReadMatDif (&newMaterial, chunk.length-6);
			break;
		case MAT_MAP:
			ReadMatMap(&newMaterial, chunk.length-6);
			break;
		default:
			fseek(m_FilePtr, chunk.length-6, SEEK_CUR);
			break;
		}
		count += chunk.length;
	}
	// 保存新的材质
	m_3DModel.pMaterials.push_back(newMaterial);
	return count;
}

// 读取材质的漫反射属性
UINT C3DSModel::ReadMatDif (tMaterial *pMat, UINT n)
{
	UINT count = 0;
	tChunk chunk = {0};
	while (count<n)
	{
		ReadChunk(&chunk);
		switch (chunk.ID)
		{
		case COLOR_BYTE:
			pMat->color[0] = ReadByte();
			pMat->color[1] = ReadByte();
			pMat->color[2] = ReadByte();
			break;
		case COLOR_FLOAT:
			debug("color float is exit!");
		default:
			fseek(m_FilePtr, chunk.length-6, SEEK_CUR);
			break;
		}
		count += chunk.length;
	}
	return count;
}

// 读取材质的纹理
UINT C3DSModel::ReadMatMap(tMaterial *pMat, UINT n)
{
	UINT count = 0;
	tChunk chunk = {0};
	while (count<n)
	{
		ReadChunk(&chunk);
		switch (chunk.ID)
		{
		case MAP_NAME:
			ReadString(&pMat->mapName);
			pMat->isTexMat = true;
			break;
		default:
			fseek(m_FilePtr, chunk.length-6, SEEK_CUR);
			break;
		}
		count += chunk.length;
	}
	return count;
}

// 释放3ds模型资源
void C3DSModel::Release(void)
{
	m_3DModel.numOfMaterials = 1;
	while (m_3DModel.pMaterials.size() != 0)
		m_3DModel.pMaterials.pop_back();
	m_3DModel.numOfObjects = 0;
	for (int nOfObj=0; nOfObj<m_3DModel.numOfObjects; nOfObj++)
	{
		delete [] m_3DModel.pObject[nOfObj].pFaces;
		delete [] m_3DModel.pObject[nOfObj].pVerts;
		delete [] m_3DModel.pObject[nOfObj].pTexVerts;
		delete [] m_3DModel.pObject[nOfObj].pNormals;
	}
	m_3DModel.pObject.clear();
}

// 计算顶点法线量
void C3DSModel::ComputeNormals(void)
{
	vector3<double> v1,v2, vNormal,vPoly[3];

	// 如果没有3ds对象则直接返回
	if (m_3DModel.numOfObjects <= 0)
		return;

	t3DObject *obj;
	int		  *index;

	for(int nOfObj=0; nOfObj<m_3DModel.numOfObjects; nOfObj++)
	{
		obj = &m_3DModel.pObject[nOfObj];
		vector3<double> *pNormals		= new vector3<double> [obj->numOfFaces];
		vector3<double> *pTempNormals	= new vector3<double> [obj->numOfFaces];
		obj->pNormals			= new vector3<double> [obj->numOfVerts];

		for(int nOfFace=0; nOfFace<obj->numOfFaces; nOfFace++)
		{
			index = obj->pFaces[nOfFace].vertIndex;

			// 三角形的3个顶点
			vPoly[0] = obj->pVerts[index[0]];
			vPoly[1] = obj->pVerts[index[1]];
			vPoly[2] = obj->pVerts[index[2]];

			// 计算这个三角形的法线量
			v1 = vPoly[0]-vPoly[1];
			v2 = vPoly[2]-vPoly[1];
			vNormal = vector3<double>::cross(v1, v2);

			pTempNormals[nOfFace] = vNormal;					// 保存未单位化的法向量
			vNormal = vector3<double>::normalize(vNormal);		// 单位化法向量
			pNormals[nOfFace] = vNormal;						// 增加到法向量数组列表
		}
		vector3<double> vSum(0.0, 0.0, 0.0);
		vector3<double> vZero(0.0, 0.0, 0.0);
		int shared=0;

		for (int nOfVert = 0; nOfVert < obj->numOfVerts; nOfVert++)			// 遍历所有顶点
		{
			for (int nOfFace = 0; nOfFace < obj->numOfFaces; nOfFace++)		// 遍历包含该顶点的面
			{
				if (obj->pFaces[nOfFace].vertIndex[0] == nOfVert || 
					obj->pFaces[nOfFace].vertIndex[1] == nOfVert || 
					obj->pFaces[nOfFace].vertIndex[2] == nOfVert)
				{
					vSum = vSum+pTempNormals[nOfFace];
					shared++;
				}
			}      
			
			obj->pNormals[nOfVert] = vSum/float(-shared);

			obj->pNormals[nOfVert] = vector3<double>::normalize(obj->pNormals[nOfVert]);	

			vSum = vZero;
			shared = 0;
		}
	
		delete [] pTempNormals;
		delete [] pNormals;
	}

}