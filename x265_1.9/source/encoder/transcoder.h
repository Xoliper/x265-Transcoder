#ifndef X265_TRANSCODER_H
#define X265_TRANSCODER_H

#define SIZE_L_MAX 4096
#define SIZE_C_MAX 1024

#define WRITE_CTU 0
#define READ_CTU  1

#define SAO_FILE 0
#define POC_FILE 1


#define TRANSCODER_ERROR -1
#define TRANSCODER_OK 0


#define POCFileHandler(h, data, size, nob, i) \
	if( i == WRITE_CTU) {WriteFile(h, data, size, nob, NULL);} \
	else if (i == READ_CTU) {ReadFile(h, data, size, nob, NULL);}


#include <Windows.h>
namespace X265_NS { class CUData; struct CUGeom; struct SAOParam; }


class Transcoder {


	public:

		Transcoder(const char * path_to_index_file);
		~Transcoder();
		void configure(int mode, int throwOnes, int throwProp);

		void workOnCTU(X265_NS::CUData * ctu, int threadId, int poc, int csp);
		void workOnSAO(X265_NS::CUData * ctu, X265_NS::SAOParam * sp, int threadId, int poc, int csp);
		void recursiveCU(X265_NS::CUData * ctu, const X265_NS::CUGeom& cuGeom, int pos);
		int getTranscoderMode();

	private:

		void recursiveTU(X265_NS::CUData * ctu, const X265_NS::CUGeom& cuGeom, int pos);
		int countNonZero(int size, X265_NS::CUData * ctu, int offset);
		
		char m_ptif[256];
		int m_mode, m_throwOnes, m_throwProp;
};


#endif