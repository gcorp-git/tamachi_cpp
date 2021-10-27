#pragma once


namespace tamachi {

	class Image {
	public:
		uint32_t width = 0;
		uint32_t height = 0;
		
		void* memory = nullptr;

		bool ok = false;

		void load( std::string filename ) {
			if ( memory ) VirtualFree( memory, 0, MEM_RELEASE );

			std::ifstream file( ( filename + ".tamachi-image" ).c_str(), std::ios::in | std::ios::binary );

			if ( !file.is_open() ) {
				std::cout << "Could not open file: " << filename << std::endl;
				return;
			}

			uint8_t version = 0;

			file.read( reinterpret_cast<char*>( &version ), sizeof(uint8_t) );

			switch ( version ) {
				case 0: {
					uint8_t char_width, char_height, char_palette_size;

					file.read( reinterpret_cast<char*>( &char_width ), sizeof(uint8_t) );
					file.read( reinterpret_cast<char*>( &char_height ), sizeof(uint8_t) );
					file.read( reinterpret_cast<char*>( &char_palette_size ), sizeof(uint8_t) );

					width = char_width ? char_width : 256;
					height = char_height ? char_height : 256;
					uint32_t palette_size = char_palette_size ? char_palette_size : 256;

					uint8_t R, G, B, A;
					uint32_t color;
					uint32_t palette[256] = {};

					for ( uint8_t i = 0; i < palette_size; ++i ) {
						file.read( reinterpret_cast<char*>( &R ), sizeof(uint8_t) );
						file.read( reinterpret_cast<char*>( &G ), sizeof(uint8_t) );
						file.read( reinterpret_cast<char*>( &B ), sizeof(uint8_t) );
						file.read( reinterpret_cast<char*>( &A ), sizeof(uint8_t) );

						// original color:

						// color = 0;
						// color = color | A;
						// color = ( color << 8 ) | R;
						// color = ( color << 8 ) | G;
						// color = ( color << 8 ) | B;

						// premultiplied by alpha:

						R = R * A / 0xFF;
						G = G * A / 0xFF;
						B = B * A / 0xFF;

						color = A << 24 | R << 16 | G << 8 | B;

						palette[ i ] = color;
					}

					uint8_t pixel;
					uint64_t size = width * height;

					memory = VirtualAlloc( 0, width * height * 4, MEM_COMMIT, PAGE_READWRITE );

					auto pos = reinterpret_cast<uint32_t*>( memory );

					for ( uint64_t i = 0; i < size; ++i ) {
						file.read( reinterpret_cast<char*>( &pixel ), sizeof(uint8_t) );
						*pos++ = palette[ pixel ];
					}

					ok = true;
				} break;
				default: {
					std::cout << "Image version is not supported: " << filename << std::endl;
					return;
				} break;
			}
		}

	};

}