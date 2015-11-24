#include "3ds.h"
#include "stlpache.h"
using namespace stlpache;

// ���캯��
C3DSModel::C3DSModel()
{
	// ��ʼ���ļ�ָ��
	m_FilePtr = NULL;

	// ����һ��Ĭ�ϵĲ��ʣ���ɫ��
	tMaterial defaultMat;
	defaultMat.isTexMat = false;
	strcpy(defaultMat.matName.string, "5DG_Default");
	defaultMat.color[0] = 192;
	defaultMat.color[1] = 192;
	defaultMat.color[2] = 192;
	m_3DModel.pMaterials.push_back(defaultMat);

	// ��ʼ������3DSģ�͵Ľṹ��
	m_3DModel.numOfMaterials = 1;
	m_3DModel.numOfObjects = 0;
}

// ��������
C3DSModel::~C3DSModel()
{
	m_3DModel.pMaterials.clear();
	m_3DModel.pObject.clear();
}

// ����3ds�ļ�
bool C3DSModel::Load(const char *strFileName)
{
	char strMessage[128] = {0};
	tChunk chunk = {0};

	// ���ļ�
	m_FilePtr = fopen(strFileName,"rb");

	// ����ļ���ʧ��
	if (!m_FilePtr)
	{
		sprintf(strMessage, "3DS�ļ� %s �����ڣ�", strFileName);
		return false;
	}

	// ��ȡ3ds�ļ��ĵ�һ��Chunk
	ReadChunk(&chunk);

	// ����Ƿ���3ds�ļ�
	if (chunk.ID != PRIMARY)
	{
		sprintf(strMessage, "��ȡ�ļ� %s ʧ�ܣ�", strFileName);
		fclose(m_FilePtr);
		return false;
	}

	// ��ʼ��ȡ3ds�ļ�
	ReadPrimary(chunk.length-6);

	// ����ÿ������ķ�����
	//yangxixi�������ļ��㷨�ߣ�Ч�ʲ��ߣ��������Լ���
	//ComputeNormals();

	// �رմ򿪵��ļ�
	fclose(m_FilePtr);
	m_FilePtr = NULL;

	return true;
}

// ���ļ��ж�ȡ1���ֽ�
BYTE C3DSModel::ReadByte(void)
{
	BYTE result = 0;
	fread(&result, 1, 1, m_FilePtr);
	return result;
}

// ���ļ��ж�ȡ2���ֽ�
WORD C3DSModel::ReadWord(void)
{
	return ReadByte() + (ReadByte()<<8);
}

// ���ļ��ж�ȡ4���ֽ�
UINT C3DSModel::ReadUint(void)
{
	return ReadWord() + (ReadWord()<<16);
}

// ���ļ��ж�ȡ������
float C3DSModel::ReadFloat(void)
{
	float result;
	fread(&result, sizeof(float), 1, m_FilePtr);
	return result;
}

// ���ļ��ж�ȡ�ַ����������ַ������ȣ�
UINT C3DSModel::ReadString(STRING *pStr)
{
	int n=0;
	while ((pStr->string[n++]=ReadByte()) != 0)
		;
	return n;
}

// ��ȡ3ds��һ��Chunk��Ϣ
void C3DSModel::ReadChunk(tChunk *pChunk)
{
	fread(&pChunk->ID, 1, 2, m_FilePtr);
	fread(&pChunk->length, 1, 4, m_FilePtr);
}

// ��ȡ3ds�ļ���ҪChunk
UINT C3DSModel::ReadPrimary(UINT n)
{
	UINT count = 0;				// ��Chunk�����Ѷ�ȡ���ֽڼ���
	tChunk chunk = {0};			// ���Ա�����Chunk������
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

// ��ȡ3ds�������༭Chunk
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

// ��ȡ3ds����
UINT C3DSModel::ReadObject(UINT n)
{
	UINT count = 0;
	tChunk chunk = {0};
	// �µ�3ds����
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
	// ����3ds����
	m_3DModel.pObject.push_back(newObject);
	return count;
}

// ��ȡ3ds������Ϣ
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
			// �����ȡ��������ֵ
			// ����ܿ��ܲ����������class�󣬲�������һ�𴢴棡�Ժ��ٸ��ˣ�����2012-04-01 00:34
			// fread(pObj->pVerts, 1, chunk.length - 8, m_FilePtr);
			// ������Ķ�ȡ���� ����y��z����ֵ(����3dMAX����ϵ������OpenGL��ͬ)2012-04-11 23:21
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
			// �����ȡ��������ֵ
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

// ��ȡ����Ϣ
UINT C3DSModel::ReadFacetInfo(t3DObject *pObj, UINT n)
{
	UINT count = 0;
	tChunk chunk = {0};
	pObj->numOfFaces = ReadWord();
	pObj->pFaces = new tFace[pObj->numOfFaces];
	memset(pObj->pFaces, 0, sizeof(tFace) * pObj->numOfFaces);

	// ��ȡ������ֵ(��4��ֵΪ3dMAXʹ�õĲ���������)
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
			ReadString(&matName);			// ��������
			t=ReadWord();					// ���ʶ�Ӧ�������
			// ���Ҷ�Ӧ�Ĳ���
			for (int i=1;i<=m_3DModel.numOfMaterials;i++)
			{
				if (strcmp(matName.string, m_3DModel.pMaterials[i].matName.string) == 0)
				{
					matID = i;
					break;
				}
			}
			// ������������ÿ����󶨲���ID
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

// ��ȡ����
UINT C3DSModel::ReadMaterial(UINT n)
{
	UINT count = 0;
	tChunk chunk = {0};
	// �µĲ���
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
	// �����µĲ���
	m_3DModel.pMaterials.push_back(newMaterial);
	return count;
}

// ��ȡ���ʵ�����������
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

// ��ȡ���ʵ�����
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

// �ͷ�3dsģ����Դ
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

// ���㶥�㷨����
void C3DSModel::ComputeNormals(void)
{
	vector3<double> v1,v2, vNormal,vPoly[3];

	// ���û��3ds������ֱ�ӷ���
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

			// �����ε�3������
			vPoly[0] = obj->pVerts[index[0]];
			vPoly[1] = obj->pVerts[index[1]];
			vPoly[2] = obj->pVerts[index[2]];

			// ������������εķ�����
			v1 = vPoly[0]-vPoly[1];
			v2 = vPoly[2]-vPoly[1];
			vNormal = vector3<double>::cross(v1, v2);

			pTempNormals[nOfFace] = vNormal;					// ����δ��λ���ķ�����
			vNormal = vector3<double>::normalize(vNormal);		// ��λ��������
			pNormals[nOfFace] = vNormal;						// ���ӵ������������б�
		}
		vector3<double> vSum(0.0, 0.0, 0.0);
		vector3<double> vZero(0.0, 0.0, 0.0);
		int shared=0;

		for (int nOfVert = 0; nOfVert < obj->numOfVerts; nOfVert++)			// �������ж���
		{
			for (int nOfFace = 0; nOfFace < obj->numOfFaces; nOfFace++)		// ���������ö������
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