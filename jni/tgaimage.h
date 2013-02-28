#ifndef TGAIMAGE_H
#define TGAIMAGE_H

struct TGAImageType {
	unsigned char* rgb;
	int width, height;
};

class TGAImage {

public:

	unsigned char* rgb;
	int width, height;

	//bool parseImage(char* filename);

};
/*

bool TGAImage::parseImage(char* filename) {
	FILE* fptr = fopen(filename, "rb");

		if (fptr == NULL) {
			//cout << "Could not open file" << endl;
			return false;
		}

		char headerbytes[18];
		for (int j=0; j < 18; j++)
			headerbytes[j] = fgetc(fptr);

		unsigned char idlength = headerbytes[0];
		unsigned char colormaptype = headerbytes[1];
		unsigned char datatypecode = headerbytes[2];
		short colormaplength = ((((short) headerbytes[6]) << 8) | 0x00FF) & (((short) headerbytes[5]) | 0xFF00);
		short width = ((((short) headerbytes[13]) << 8) | 0x00FF) & (((short) headerbytes[12]) | 0xFF00);
		short height = ((((short) headerbytes[15]) << 8) | 0x00FF) & (((short) headerbytes[14]) | 0xFF00);

		unsigned char bitdepth = headerbytes[16];

		this->width = (int) width;
		this->height = (int) height;

		//cout << filename << " " << width << " " << height << " " << bitdepth << endl;

		this->rgb = (unsigned char*) malloc(sizeof(unsigned char)*width*height*4);

		// if anything is not up to par, just throw an exception from the parser
		if(this->rgb == NULL) {
			fclose(fptr);
			//cout << "Unable to malloc memory" << endl;;
			return false;
		}
		if (datatypecode != 2 && datatypecode != 10) {
			fclose(fptr);
			//cout << "Can only handle this type 2 and 10" << endl;
			return false;
		}
		if (bitdepth != 24 && bitdepth != 32) {
			fclose(fptr);
			//cout << "Can only handle pixel depths of 24 and 32" << endl;
			return false;
		}
		if (colormaptype != 0 && colormaptype != 1) {
			fclose(fptr);
			//cout << "Can only handle colour map types of 0 and 1" << endl;
			return false;
		}

		unsigned int skipover = idlength;
		skipover += colormaplength;
		fseek(fptr, skipover, SEEK_CUR);

		unsigned int byteCount = bitdepth / 8;
		unsigned char p[5];

		int n =0;
		while (n < width*height) {
			if (datatypecode == 2) {                     // Uncompressed
				if (fread(p,1,byteCount,fptr) != byteCount) {
					fclose(fptr);
					free(this->rgb);
				//	cout << "Unexpected end of file" << endl;
					return false;
				}

				this->rgb[n*4+0] = p[2];
				this->rgb[n*4+1] = p[1];
				this->rgb[n*4+2] = p[0];
				this->rgb[n*4+3] = p[3];
				//tgaFile.data[n].a = p[3];

				//if (byteCount == 3) tgaFile.data[n].a = 255;
				n++;
			} else if (datatypecode == 10) {             // Compressed
				if (fread(p,1,byteCount+1,fptr) != byteCount+1) {
					fclose(fptr);
					//cout << "Unexpected end of file" << endl;
					return false;
				}
				int j = p[0] & 0x7f;

				this->rgb[n*3+0] = p[3];
				this->rgb[n*3+1] = p[2];
				this->rgb[n*3+2] = p[1];
				this->rgb[n*4+3] = (byteCount == 4)?p[4]:255;

				n++;

				if (p[0] & 0x80) {         // RLE chunk
					for (int i=0;i<j;i++) {
						this->rgb[n*3+0] = p[3];
						this->rgb[n*3+1] = p[2];
						this->rgb[n*3+2] = p[1];
						this->rgb[n*4+3] = (byteCount == 4)?p[4]:255;
						n++;
					}
				} else {                   // Normal chunk
					for (int i=0;i<j;i++) {
						if (fread(p,1,byteCount,fptr) != byteCount) {
							fclose(fptr);
							//cout << "Unexpected end of file" << endl;
							return false;
						}
						this->rgb[n*4+0] = p[3];
						this->rgb[n*4+1] = p[2];
						this->rgb[n*4+2] = p[1];
						this->rgb[n*4+3] = (byteCount == 4)?p[4]:255;
						n++;
					}
				}
			}
		}

		return true;
}

*/


/********
*
*    OLD
*
*
*********/



/*

bool parseImage(TGAImageType* image, char* filename) {

	FILE* fptr = fopen(filename, "rb");

	if (fptr == NULL) {
		//cout << "Could not open file" << endl;
		return false;
	}

	char headerbytes[18];
	for (int j=0; j < 18; j++)
		headerbytes[j] = fgetc(fptr);

	unsigned char idlength = headerbytes[0];
	unsigned char colormaptype = headerbytes[1];
	unsigned char datatypecode = headerbytes[2];
	short colormaplength = ((((short) headerbytes[6]) << 8) | 0x00FF) & (((short) headerbytes[5]) | 0xFF00);
	short width = ((((short) headerbytes[13]) << 8) | 0x00FF) & (((short) headerbytes[12]) | 0xFF00);
	short height = ((((short) headerbytes[15]) << 8) | 0x00FF) & (((short) headerbytes[14]) | 0xFF00);

	unsigned char bitdepth = headerbytes[16];

	image->width = (int) width;
	image->height = (int) height;

	//cout << filename << " " << width << " " << height << " " << bitdepth << endl;

	image->rgb = (unsigned char*) malloc(sizeof(unsigned char)*width*height*4);

	// if anything is not up to par, just throw an exception from the parser
	if(image->rgb == NULL) {
		fclose(fptr);
		//cout << "Unable to malloc memory" << endl;;
		return false;
	}
	if (datatypecode != 2 && datatypecode != 10) {
		fclose(fptr);
		//cout << "Can only handle image type 2 and 10" << endl;
		return false;
	}
	if (bitdepth != 24 && bitdepth != 32) {
		fclose(fptr);
		//cout << "Can only handle pixel depths of 24 and 32" << endl;
		return false;
	}
	if (colormaptype != 0 && colormaptype != 1) {
		fclose(fptr);
		//cout << "Can only handle colour map types of 0 and 1" << endl;
		return false;
	}

	unsigned int skipover = idlength;
	skipover += colormaplength;
	fseek(fptr, skipover, SEEK_CUR);

	unsigned int byteCount = bitdepth / 8;
	unsigned char p[5];

	int n =0;
	while (n < width*height) {
		if (datatypecode == 2) {                     //Uncompressed
			if (fread(p,1,byteCount,fptr) != byteCount) {
				fclose(fptr);
				free(image->rgb);
			//	cout << "Unexpected end of file" << endl;
				return false;
			}

			image->rgb[n*4+0] = p[2];
			image->rgb[n*4+1] = p[1];
			image->rgb[n*4+2] = p[0];
			image->rgb[n*4+3] = p[3];
			//tgaFile.data[n].a = p[3];

			//if (byteCount == 3) tgaFile.data[n].a = 255;
			n++;
		} else if (datatypecode == 10) {             // Compressed
			if (fread(p,1,byteCount+1,fptr) != byteCount+1) {
				fclose(fptr);
				//cout << "Unexpected end of file" << endl;
				return false;
			}
			int j = p[0] & 0x7f;

			image->rgb[n*3+0] = p[3];
			image->rgb[n*3+1] = p[2];
			image->rgb[n*3+2] = p[1];
			image->rgb[n*4+3] = (byteCount == 4)?p[4]:255;

			n++;

			if (p[0] & 0x80) {         // RLE chunk
				for (int i=0;i<j;i++) {
					image->rgb[n*3+0] = p[3];
					image->rgb[n*3+1] = p[2];
					image->rgb[n*3+2] = p[1];
					image->rgb[n*4+3] = (byteCount == 4)?p[4]:255;
					n++;
				}
			} else {                   // Normal chunk
				for (int i=0;i<j;i++) {
					if (fread(p,1,byteCount,fptr) != byteCount) {
						fclose(fptr);
						//cout << "Unexpected end of file" << endl;
						return false;
					}
					image->rgb[n*4+0] = p[3];
					image->rgb[n*4+1] = p[2];
					image->rgb[n*4+2] = p[1];
					image->rgb[n*4+3] = (byteCount == 4)?p[4]:255;
					n++;
				}
			}
		}
	}

	return true;
}
*/
#endif



