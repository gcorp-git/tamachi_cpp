#pragma once

#include "image.cpp"


namespace tamachi {
	namespace images {

		auto _storage = new Storage<std::string, Image>();

		void load( std::vector<std::string> paths ) {
			for ( auto path : paths ) {
				auto image = _storage->create();

				image->load( path );

				_storage->set( path, image );
			}
		}

		Image* get( std::string path ) {
			return _storage->get( path );
		}

		void reset() {
			_storage->clear();
		}

	}
}