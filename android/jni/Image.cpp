/***
 * -*- C++ -*-
 * -*- UTF-8 -*-
 *
 * Copyright (C),
 * @file
 * @brief
 * @author:
 * 		<name>		<email>
 * 		kins ren	kins.ren@me.com
 * @version
 * @date
 * @warning
 * @History:
 * 		<author>	<time>	<desc>
*/

#include <png.h>

#include "global.h"
#include "stringutil.h"
#include "Image.h"


// @todo: 检查PNG图片不完整,读取越界
static void png_raw_read(png_structp png, png_bytep data, png_size_t size)
{
	unsigned char **raws = (unsigned char **)png_get_io_ptr(png);
	memcpy(data, *raws, size);
	*raws = *raws + size;

	LOG("png raws read\n");
}

void Image::createTexture(const void *input, unsigned int size)
{
	const char *filename = m_src.c_str();

	do{
		const void *raws = input;
		
		png_byte header[8];
        memcpy(header, raws, 8);

		// 读取图片头,判断是否PNG文件
		if( png_sig_cmp(header, 0, 8) )
		{
			LOG("Not a png file : %s", filename);
			break;
		}

		// 创建PNG读取结构
		png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
		if( !png_ptr )
		{
			LOG("Unable to create png struct : %s", filename);
			break;
		}

		// 创建PNG信息结构
		png_infop info_ptr = png_create_info_struct(png_ptr);
		if( !info_ptr )
		{
			png_destroy_read_struct(&png_ptr, (png_infopp) NULL, (png_infopp) NULL);
			LOG("Unable to create png info : %s", filename);
			break;
		}

		if( setjmp(png_jmpbuf(png_ptr)) )
		{
			LOG("Error during setjmp : %s", filename);
			png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp) NULL);
			break;
		}
		
		// 设置数据读取函数
		void *inputCopy = (void *)input;
        png_set_read_fn(png_ptr, &inputCopy, png_raw_read);

		// 设置PNG数据宽度处理 RGB/RGBA 24/32bit
		//png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_EXPAND | PNG_TRANSFORM_PACKING
		//								| PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_GRAY_TO_RGB, 0);
		png_read_info(png_ptr, info_ptr);

		// 读取图片信息
		int bit_depth, color_type;
		png_uint_32 twidth, theight;

		png_get_IHDR(png_ptr, info_ptr, &twidth, &theight, &bit_depth, &color_type, NULL, NULL, NULL);
		hasAlpha = ( color_type & PNG_COLOR_MASK_ALPHA ) ? true:false;

		LOG("PNG width=%u height=%u bit_depth=%d alpha=%d", twidth, theight, bit_depth, hasAlpha);
		

		// Update the png info struct.
		//png_read_update_info(png_ptr, info_ptr);

		int rowbytes = png_get_rowbytes(png_ptr, info_ptr);
		LOG("PNG rowbytes:%d", rowbytes);
		
		int bytes_per_component = hasAlpha ? 4:3;

		width = twidth;
		height = theight;
		POTWidth = computePOT(width);
		POTHeight = computePOT(height);

		png_byte *image_data = new png_byte[POTWidth * POTHeight * bytes_per_component];
		if( !image_data )
		{
			png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp) NULL);
			LOG("Unable to allocate image_data while loading %s ", filename);
			break;
		}

		// 创建并设置行指针
		png_bytep *row_pointers = new png_bytep[theight];
		if( !row_pointers )
		{
			png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp) NULL);
			delete[] image_data;
			LOG("Unable to allocate row_pointer while loading %s ", filename);
			break;
		}

		for( int i = 0; i < theight; i++ )
		{
			row_pointers[i] = image_data + i * POTWidth * bytes_per_component;
		}

		png_read_image(png_ptr, row_pointers);

		png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp) NULL);
		delete[] row_pointers;

		// 如果有alpha值,开始预乘
		if( hasAlpha )
		{
			for( int i=0; i<theight; i++ )
			{
				for( int j=0; j<twidth; j++ )
				{
					unsigned char *pixel = (unsigned char *)(image_data + (i * POTWidth + j) * bytes_per_component);
					*((unsigned int *)pixel) = CC_RGB_PREMULTIPLY_APLHA( pixel[0], pixel[1], pixel[2], pixel[3] );
				}
			}
		}

        setupTexture(image_data);

        LOG("Loading PNG: %s finish",filename);

		delete[] image_data;
	}while(false);
}
