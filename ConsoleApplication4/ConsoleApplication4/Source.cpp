#define _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_DEPRECATE
#define _CRT_NONSTDC_NO_DEPRECATE
#pragma warning(disable : 4996)

#include <iostream>
#include <stdio.h>
extern "C"
{
#include <jpeglib.h>
}
#include <stdlib.h>

class JPEG_Decod_Encod
{
private:
	unsigned char *raw_image = NULL;
	int width;
	int height;
	int quality_file = 100;
	int bytes_per_pixel = 3;
	J_COLOR_SPACE color_space = JCS_RGB;
	char *fileIn, *fileOut;
	int read_jpeg_file(char *filename);
	int write_jpeg_file(char *filename);
public:
	int rw_file();
	JPEG_Decod_Encod(char *filenameIn, char *filenameOut, int quality);
};

JPEG_Decod_Encod::JPEG_Decod_Encod(char *filenameIn, char *filenameOut, int quality)
{
	fileIn = filenameIn;
	fileOut = filenameOut;
	quality_file = quality;
}

int JPEG_Decod_Encod::read_jpeg_file(char *filename)
{
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;
	JSAMPROW row_pointer[1];

	FILE *infile = fopen(filename, "rb");
	unsigned long location = 0;

	if (!infile)
	{
		printf("Error opening jpeg file %s\n!", filename);
		return -1;
	}
	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&cinfo);
	jpeg_stdio_src(&cinfo, infile);
	jpeg_read_header(&cinfo, TRUE);

	width = cinfo.image_width;
	height = cinfo.image_height;

	jpeg_start_decompress(&cinfo);

	raw_image = new unsigned char[cinfo.output_width*cinfo.output_height*cinfo.num_components];
	row_pointer[0] = new unsigned char[cinfo.output_width*cinfo.num_components];
	while (cinfo.output_scanline < cinfo.image_height)
	{
		jpeg_read_scanlines(&cinfo, row_pointer, 1);
		for (int i = 0; i<cinfo.image_width*cinfo.num_components; i++)
			raw_image[location++] = row_pointer[0][i];
	}

	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);
	delete row_pointer[0];
	fclose(infile);
	return 0;
}

int JPEG_Decod_Encod::write_jpeg_file(char *filename)
{
	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr;

	JSAMPROW row_pointer[1];
	FILE *outfile = fopen(filename, "wb");

	if (!outfile)
	{
		printf("Error opening output jpeg file %s\n!", filename);
		return -1;
	}
	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_compress(&cinfo);
	jpeg_stdio_dest(&cinfo, outfile);


	cinfo.image_width = width;
	cinfo.image_height = height;
	cinfo.input_components = bytes_per_pixel;
	cinfo.in_color_space = color_space;

	jpeg_set_defaults(&cinfo);
	cinfo.num_components = 3;
	cinfo.dct_method = JDCT_FLOAT;
	jpeg_set_quality(&cinfo, quality_file, TRUE);
	jpeg_start_compress(&cinfo, TRUE);
	while (cinfo.next_scanline < cinfo.image_height)
	{
		row_pointer[0] = &raw_image[cinfo.next_scanline * cinfo.image_width * cinfo.input_components];
		jpeg_write_scanlines(&cinfo, row_pointer, 1);
	}
	jpeg_finish_compress(&cinfo);
	jpeg_destroy_compress(&cinfo);
	fclose(outfile);
	return 0;
}

int JPEG_Decod_Encod::rw_file()
{
	if (read_jpeg_file(fileIn) == 0)
	{
		if (write_jpeg_file(fileOut) != 0) return -1;
	}
	else return -1;
}

int main(int argc, char *argv[])
{
	int quality = 100;
	char *infilename = "3.jpg", *outfilename = "3_copy.jpg";
	if (argc != 4)
	{
		std::cout << "Usage: ConsoleApplication4.exe infile.jpg outfile.jpg (int)quality" << std:: endl;
	}
	else
	{
		infilename = argv[1];
		outfilename = argv[2];
		quality = atoi(argv[3]);
		JPEG_Decod_Encod jpegfile(infilename, outfilename, quality);
		jpegfile.rw_file();
	}
	return 0;
}
