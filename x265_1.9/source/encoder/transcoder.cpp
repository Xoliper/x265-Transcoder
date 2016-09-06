#include "transcoder.h"
#include "cudata.h"
#include "mv.h"
#include "search.h"
#include "common.h"
#include <string>


	//===================================================================================
	const X265_NS::CUData* X265_NS::CUData::getCuNeighbor(uint32_t absIdx, uint32_t & neighbor, uint32_t log2CUSize, int vert, int hor)
	{
		uint32_t raster = g_zscanToRaster[absIdx];
		uint32_t x, y;
		y = raster / 16;
		x = raster % 16;

		uint32_t width_in_part = (1 << (log2CUSize - 2));
		if (width_in_part < 1)
		{
			width_in_part = 1;
			printf("\nSomething goes wrong.\n");
			system("pause");
		}
		uint32_t height_in_part = width_in_part;

		uint32_t x_neighbor, y_neighbor;
		y_neighbor = y + vert*height_in_part;
		x_neighbor = x + hor*width_in_part;
		//--------------------------------------------
		int off = 0;
		/* 0 = is act CTU

		-4; -3; -2;
		-1;  0; +1;
		+2; +3; +4;
		*/
		off = 3 * offset(x_neighbor);
		off += offset(y_neighbor);
		neighbor = y_neighbor * 16 + x_neighbor;

		return this->neighborCTU(off);

	}
	//===================================================================================
	const X265_NS::CUData* X265_NS::CUData::neighborCTU(int offset) {
		/* 0 = is act CTU
		-4; -3; -2;
		-1;  0; +1;
		+2; +3; +4;
		*/
		switch (offset)
		{
		case -4:
			return this->m_cuAboveLeft;
			break;
		case -3:
			return this->m_cuAbove;
			break;
		case -2:
			return this->m_cuAboveRight;
			break;
		case -1:
			return this->m_cuLeft;
			break;
		case 0:
			return this;
			break;
		case 1:
			return this->m_cuRight;
			break;
		case 2:
			return this->m_cuBelowLeft;
			break;
		case 3:
			return this->m_cuBelow;
			break;
		case 4:
			return this->m_cuBelowRight;
			break;
		default:
			printf("\nBad idx of neighbor CTU.");
			system("pause");
			break;
		}
	}
	//===================================================================================
	int X265_NS::CUData::offset(uint32_t& pos_neighbor)
	{
		if (pos_neighbor > 15) {
			pos_neighbor -= 16;
			return 1;
		}
		else if (pos_neighbor < 0) {
			pos_neighbor += 16;
			return-1;
		}
		else
		{
			return 0;
		}
	}
	//===================================================================================


	Transcoder::Transcoder(const char * path_to_index_file)
	{
		int size = strlen(path_to_index_file);
		//m_ptif = new char[size]; 
		//memset(m_ptif, '-1', size);
		strncpy(m_ptif, path_to_index_file, size);
		m_ptif[size] = '\0';
		
		m_throwOnes = 0;
		m_throwProp = 100;
	}


	Transcoder::~Transcoder(){
	}


	void Transcoder::configure(int mode, int throwOnes, int throwProp) {
		std::string dupa = "dupa";
		dupa = dupa * "5";

		m_mode = mode;
		m_throwOnes = throwOnes;
		m_throwProp = throwProp;

		if (m_mode == TRANSCODER_ERROR)
		{
			X265_NS::x265_log(false, X265_LOG_ERROR, "Selected transcoder mode is invalid \n");
			system("PAUSE"); exit(TRANSCODER_ERROR);
		}

	}

	/*
	bool Transcoder::checkIntra(X265_NS::CUData * parentCTU, const X265_NS::CUGeom& cuGeom, int mode) {

		int oldAbsPartIdx = cuGeom.absPartIdx;
		int log2CUSize = cuGeom.log2CUSize;
		int masterFlag = true;

		int goSize = (1 << log2CUSize) / (1 << log2CUSize-2);
		uint32_t edgeSize = (1 << (log2CUSize - 2));


		//for (int i = 0; i <1; i++) {

			//Obliczanie nowej pozycji
			coords crd;
			uint32_t absPartIdx = X265_NS::g_zscanToRaster[oldAbsPartIdx];
			uint32_t xInRaster = absPartIdx / 16;
			uint32_t yInRaster = absPartIdx % 16;


			if (mode == RIGHT) {
				xInRaster += edgeSize;
			}
			else if (mode == RIGHTBELOW) {
				xInRaster += edgeSize;
				yInRaster += edgeSize;
			}
			else if (mode == BELOW) {
				yInRaster += edgeSize;
			}
			else if (mode == LEFTBELOW) {
				xInRaster -= edgeSize;
				yInRaster += edgeSize;
			}


			crd.x = xInRaster;
			crd.y = yInRaster;
			if (yInRaster * 16 + xInRaster < 256) {
				crd.error = TRANSCODER_OK;
			}


			//Pozyskanie interesuj¹cego nas CU
			const X265_NS::CUData * ctu;
			if (crd.error != TRANSCODER_ERROR) {

				int offset;
				offset = 3 * getOffset(crd.x);
				offset += getOffset(crd.y);

				switch (offset) {
				case -4: ctu = parentCTU->m_cuAboveLeft; break;
				case -3: ctu = parentCTU->m_cuAbove; break;
				case -2: ctu = parentCTU->m_cuAboveRight; break;
				case -1: ctu = parentCTU->m_cuLeft; break;
				case 0:  ctu = parentCTU; break;
				case 1:  ctu = parentCTU->m_cuRight; break;
				case 2:  ctu = parentCTU->m_cuBelowLeft; break;
				case 3:  ctu = parentCTU->m_cuBelow; break;
				case 4:  ctu = parentCTU->m_cuBelowRight; break;
				default: ctu = NULL;
				}

			}
			else {
				ctu = NULL;
			}

			//Sprawdzanie 
			if (ctu != NULL && ctu->isInter(crd.y * 16 + crd.x)) {
				return false;
			}
			else if (ctu == NULL) {
				return false;
			}

			//Wystarczy obliczyæ tylko to
			//oldAbsPartIdx += edgeSize;

		//}

		return masterFlag;
	}
	*/

	


	void Transcoder::recursiveCU(X265_NS::CUData * parentCTU, const X265_NS::CUGeom& cuGeom, int pos)
	{
		if (cuGeom.depth < parentCTU->m_cuDepth[cuGeom.absPartIdx] && cuGeom.depth < NUM_CU_DEPTH)//g_maxCUDepth
		{

			for (uint32_t subPartIdx = 0; subPartIdx < 4; subPartIdx++)
			{
				const X265_NS::CUGeom& childGeom = *(&cuGeom + cuGeom.childOffset + subPartIdx);
				if (childGeom.flags & X265_NS::CUGeom::PRESENT) {
					int size = (1 << childGeom.log2CUSize); //Same as: 1 << (MAX_LOG2_CU_SIZE - cuGeom.depth);
					size *= size;
					recursiveCU(parentCTU, childGeom, pos + subPartIdx*size);
				} 
			}
		}
		else
		{



			bool below, belowRight, right, belowLeft;
			below = belowRight = right = belowLeft = true;


			//----------------------------------------------------------------------
			uint32_t neighborIdx[4] = { 0,0,0,0 };

			const X265_NS::CUData* ctuRight = parentCTU->getCuNeighbor(cuGeom.absPartIdx, neighborIdx[0], cuGeom.log2CUSize, 0, 1);
			const X265_NS::CUData* ctuBelowRight = parentCTU->getCuNeighbor(cuGeom.absPartIdx, neighborIdx[1], cuGeom.log2CUSize, 1, 1);
			const X265_NS::CUData* ctuBelow = parentCTU->getCuNeighbor(cuGeom.absPartIdx, neighborIdx[2], cuGeom.log2CUSize, 1, 0);
			const X265_NS::CUData* ctuBelowLeft = parentCTU->getCuNeighbor(cuGeom.absPartIdx, neighborIdx[3], cuGeom.log2CUSize, 1, -1);

			/*for (size_t i = 0; i < 4; i++)
			{
			neighborIdx[i] = g_rasterToZscan[neighborIdx[i]];
			}
			*/
			uint32_t x, y, x_neighbor, y_neighbor, iterator;
			int sizeInPart = 1 << (cuGeom.log2CUSize - 2);
			if (ctuRight != NULL)
			{
				//right = !ctuRight->isInter(neighborIdx[0]);
				//neighborIdx[0] = g_zscanToRaster[neighborIdx[0]];

				y = neighborIdx[0] / 16;
				x = neighborIdx[0] % 16;

				for (size_t i = 0; i < sizeInPart; i++)
				{
					y_neighbor = y + i;
					x_neighbor = x;
					iterator = y_neighbor * 16 + x_neighbor;
					right = !ctuRight->isInter(X265_NS::g_rasterToZscan[iterator]);
					if (!right)
					{
						break;
					}
				}
			}
			if (ctuBelowRight != NULL)
			{
				belowRight = !ctuBelowRight->isInter(X265_NS::g_rasterToZscan[neighborIdx[1]]);
			}
			if (ctuBelow != NULL)
			{
				//right = !ctuRight->isInter(neighborIdx[0]);
				//neighborIdx[0] = g_zscanToRaster[neighborIdx[0]];
				y = neighborIdx[2] / 16;
				x = neighborIdx[2] % 16;
				for (size_t i = 0; i < sizeInPart; i++)
				{
					y_neighbor = y;
					x_neighbor = x + i;
					iterator = y_neighbor * 16 + x_neighbor;
					below = !ctuBelow->isInter(X265_NS::g_rasterToZscan[iterator]);
					if (!below)
					{
						break;
					}
				}
			}
			if (ctuBelowLeft != NULL)
			{
				belowLeft = !ctuBelowLeft->isInter(X265_NS::g_rasterToZscan[neighborIdx[3]]);
				y = neighborIdx[3] / 16;
				x = neighborIdx[3] % 16;
				x_neighbor += sizeInPart - 1;
				y_neighbor = y;
				iterator = y_neighbor * 16 + x_neighbor;
				bool belowLeftnear = !ctuBelowLeft->isInter(X265_NS::g_rasterToZscan[iterator]);
				if (!belowLeftnear)
				{
					belowLeft = false;
				}
			}
			if (!(cuGeom.flags & X265_NS::CUGeom::INTRA) && below && belowRight && right&& belowLeft)
			{
				X265_NS::CUGeom x = X265_NS::CUGeom(cuGeom);
				x.depth = 0;
				recursiveTU(parentCTU, x, pos);
			}






			/*int masterFlag = true;
			if (checkIntra(parentCTU, cuGeom, RIGHT) != true)	   masterFlag = false;
			if (checkIntra(parentCTU, cuGeom, LEFTBELOW) != true)  masterFlag = false;
			if (checkIntra(parentCTU, cuGeom, BELOW) != true)	   masterFlag = false;
			if (checkIntra(parentCTU, cuGeom, RIGHTBELOW) != true) masterFlag = false;


			if (masterFlag) {
				X265_NS::CUGeom tuGeom = X265_NS::CUGeom(cuGeom); tuGeom.depth = 0;
				if (!(cuGeom.flags & X265_NS::CUGeom::INTRA)) { // && parentCTU->isInter(cuGeom.absPartIdx)
					recursiveTU(parentCTU, tuGeom, pos);
				}
			}*/
		
			
		}

		return;
	}




	void Transcoder::recursiveTU(X265_NS::CUData * parentCTU, const X265_NS::CUGeom& tuGeom, int pos) {

		if (tuGeom.depth < parentCTU->m_tuDepth[tuGeom.absPartIdx] && tuGeom.depth < NUM_CU_DEPTH - 1)
		{

			for (uint32_t subPartIdx = 0; subPartIdx < 4; subPartIdx++)
			{
				X265_NS::CUGeom tmpGeom = X265_NS::CUGeom(tuGeom);
				--tmpGeom.log2CUSize;

				tmpGeom.absPartIdx += subPartIdx*(4 / (1 << tmpGeom.log2CUSize));
				tmpGeom.numPartitions = tmpGeom.numPartitions / 4;
				tmpGeom.depth++;

				int size = 1 << tmpGeom.log2CUSize;
				size *= size;
				recursiveTU(parentCTU, tmpGeom, pos + subPartIdx*size);

			}
			return;
		}
		else
		{


			int size = 1 << tuGeom.log2CUSize;
			size *= size;
			int numSig = countNonZero(size, parentCTU, pos);

			int deleteCounter = 0;
			int proportions; m_throwProp != 0 ? proportions = 100 / m_throwProp : proportions = 100;

			if (numSig > 1) {
				for (int i = (size - 1); i > size/proportions; i--) {
					
					if (m_throwOnes != 0) {
						if (deleteCounter > m_throwOnes) break;
					}

					if (parentCTU->m_trCoeff[0][pos + i] == -1 || parentCTU->m_trCoeff[0][pos + i] == 1) {
						parentCTU->m_trCoeff[0][pos + i] = 0;
						++deleteCounter;
						break;
					}

				}
			}


		}
	
		return;
	}



	int Transcoder::countNonZero(int size, X265_NS::CUData * ctu, int offset)
	{
		int numSig = 0;
		for (int i = 0; i < size-1; i++) {
			if (ctu->m_trCoeff[0][i + offset] == 1 || ctu->m_trCoeff[0][i+offset] == -1) ++numSig;
		}
		return numSig;
	}


	int Transcoder::getTranscoderMode()
	{
		return m_mode;
	}

	void Transcoder::workOnSAO(X265_NS::CUData * ctu, X265_NS::SAOParam * sp, int threadId, int poc, int csp)
	{

		std::string file(m_ptif); file += "_" + std::to_string(poc) + ".sao";
		HANDLE hFile = CreateFile(file.c_str(), (GENERIC_READ | GENERIC_WRITE), (FILE_SHARE_WRITE | FILE_SHARE_READ), NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			X265_NS::x265_log(false, X265_LOG_ERROR, "Transcoder cannot open .sao file! \n");
		}

		long jump = sizeof(bool) * 2 + sizeof(X265_NS::SaoCtuParam) * (csp != X265_CSP_I400 ? 3 : 1);
		jump *= ctu->m_cuAddr;

		//Execute JUMP!
		LARGE_INTEGER li; li.QuadPart = jump;
		SetFilePointerEx(hFile, li, NULL, FILE_BEGIN);

		//Load SAO data
		DWORD ionumber;
		POCFileHandler(hFile, &sp->bSaoFlag[0], sizeof(bool), &ionumber, m_mode);
		POCFileHandler(hFile, &sp->bSaoFlag[1], sizeof(bool), &ionumber, m_mode);
		POCFileHandler(hFile, &sp->ctuParam[0][ctu->m_cuAddr], sizeof(X265_NS::SaoCtuParam), &ionumber, m_mode);
		if (csp != X265_CSP_I400)
		{
			POCFileHandler(hFile, &sp->ctuParam[1][ctu->m_cuAddr], sizeof(X265_NS::SaoCtuParam), &ionumber, m_mode);
			POCFileHandler(hFile, &sp->ctuParam[2][ctu->m_cuAddr], sizeof(X265_NS::SaoCtuParam), &ionumber, m_mode);
		}

		CloseHandle(hFile);

	}

	void Transcoder::workOnCTU(X265_NS::CUData * ctu, int threadId, int poc, int csp)
	{

		std::string file(m_ptif); file += "_" + std::to_string(poc) + ".poc";
		HANDLE hFile = CreateFile(file.c_str(), (GENERIC_READ | GENERIC_WRITE), (FILE_SHARE_WRITE | FILE_SHARE_READ), NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			X265_NS::x265_log(false, X265_LOG_ERROR, "Transcoder cannot open .poc file! \n");
		}



		//Calculate position and execute that jump
		long jump = sizeof(uint8_t) * 10 * ctu->m_numPartitions + sizeof(int8_t) * 2 * ctu->m_numPartitions + sizeof(uint32_t) * 1 + sizeof(uint8_t) * 2 * ctu->m_numPartitions +
			sizeof(coeff_t)*SIZE_L_MAX + sizeof(X265_NS::MV)*ctu->m_numPartitions * 4 + sizeof(uint8_t)*ctu->m_numPartitions + sizeof(uint32_t) * 7 + sizeof(int8_t)*ctu->m_numPartitions;
		if (csp != X265_CSP_I400) jump += sizeof(uint8_t) * 4 * ctu->m_numPartitions + sizeof(coeff_t)*SIZE_C_MAX * 2;
		jump *= ctu->m_cuAddr;

		//Execute JUMP!
		LARGE_INTEGER li; li.QuadPart = jump;
		SetFilePointerEx(hFile, li, NULL, FILE_BEGIN);

		//Lets write/read something!
		DWORD ionumber;
		POCFileHandler(hFile, &ctu->m_cuAddr, sizeof(uint32_t), &ionumber, m_mode);
		POCFileHandler(hFile, &ctu->m_absIdxInCTU, sizeof(uint32_t), &ionumber, m_mode); //UWAGA
		POCFileHandler(hFile, &ctu->m_cuPelX, sizeof(uint32_t), &ionumber, m_mode);
		POCFileHandler(hFile, &ctu->m_cuPelY, sizeof(uint32_t), &ionumber, m_mode);
		POCFileHandler(hFile, &ctu->m_numPartitions, sizeof(uint32_t), &ionumber, m_mode);
		POCFileHandler(hFile, &ctu->m_chromaFormat, sizeof(uint32_t), &ionumber, m_mode);
		POCFileHandler(hFile, &ctu->m_hChromaShift, sizeof(uint32_t), &ionumber, m_mode);
		POCFileHandler(hFile, &ctu->m_vChromaShift, sizeof(uint32_t), &ionumber, m_mode);
		POCFileHandler(hFile, ctu->m_qp, sizeof(int8_t)*ctu->m_numPartitions, &ionumber, m_mode); //TUTAJ
		POCFileHandler(hFile, ctu->m_log2CUSize, sizeof(uint8_t)*ctu->m_numPartitions, &ionumber, m_mode);
		POCFileHandler(hFile, ctu->m_lumaIntraDir, sizeof(uint8_t)*ctu->m_numPartitions, &ionumber, m_mode);
		POCFileHandler(hFile, ctu->m_refIdx[0], sizeof(int8_t)*ctu->m_numPartitions, &ionumber, m_mode);
		POCFileHandler(hFile, ctu->m_refIdx[1], sizeof(int8_t)*ctu->m_numPartitions, &ionumber, m_mode);
		POCFileHandler(hFile, ctu->m_cuDepth, sizeof(uint8_t)*ctu->m_numPartitions, &ionumber, m_mode);
		POCFileHandler(hFile, ctu->m_predMode, sizeof(uint8_t)*ctu->m_numPartitions, &ionumber, m_mode);
		POCFileHandler(hFile, ctu->m_partSize, sizeof(uint8_t)*ctu->m_numPartitions, &ionumber, m_mode);
		POCFileHandler(hFile, ctu->m_mergeFlag, sizeof(uint8_t)*ctu->m_numPartitions, &ionumber, m_mode);
		POCFileHandler(hFile, ctu->m_interDir, sizeof(uint8_t)*ctu->m_numPartitions, &ionumber, m_mode);
		POCFileHandler(hFile, ctu->m_mvpIdx[0], sizeof(uint8_t)*ctu->m_numPartitions, &ionumber, m_mode);
		POCFileHandler(hFile, ctu->m_mvpIdx[1], sizeof(uint8_t)*ctu->m_numPartitions, &ionumber, m_mode);
		POCFileHandler(hFile, ctu->m_tuDepth, sizeof(uint8_t)*ctu->m_numPartitions, &ionumber, m_mode);

		for (int i = 0; i < (csp == X265_CSP_I400 ? 1 : 3); i++)
		{
			POCFileHandler(hFile, ctu->m_transformSkip[i], sizeof(uint8_t)*ctu->m_numPartitions, &ionumber, m_mode);
		}
		for (int i = 0; i < (csp == X265_CSP_I400 ? 1 : 3); i++)
		{
			POCFileHandler(hFile, ctu->m_cbf[i], sizeof(uint8_t)*ctu->m_numPartitions, &ionumber, m_mode);
		}

		POCFileHandler(hFile, ctu->m_chromaIntraDir, sizeof(uint8_t)*ctu->m_numPartitions, &ionumber, m_mode);

		POCFileHandler(hFile, ctu->m_trCoeff[0], sizeof(coeff_t)*SIZE_L_MAX, &ionumber, m_mode);
		if (csp != X265_CSP_I400)
		{
			POCFileHandler(hFile, ctu->m_trCoeff[1], sizeof(coeff_t)*SIZE_C_MAX, &ionumber, m_mode);
			POCFileHandler(hFile, ctu->m_trCoeff[2], sizeof(coeff_t)*SIZE_C_MAX, &ionumber, m_mode);
		}


		for (size_t n = 0; n < 2; n++)
		{
			POCFileHandler(hFile, ctu->m_mv[n], sizeof(X265_NS::MV)*ctu->m_numPartitions, &ionumber, m_mode);
		}
		for (size_t n = 0; n < 2; n++)
		{
			POCFileHandler(hFile, ctu->m_mvd[n], sizeof(X265_NS::MV)*ctu->m_numPartitions, &ionumber, m_mode);
		}


		CloseHandle(hFile);
		
	}
