NETWORK_LIBRARY = s21_client_library.a
OUTPUT = s21_desktop

all: desktop

desktop: $(NETWORK_LIBRARY)
	cd cmake && \
	cmake CMakeLists.txt && \
	make
	cp cmake/$(OUTPUT) .

$(NETWORK_LIBRARY):
	cd ../../client_library && make
	cp ../../client_library/$(NETWORK_LIBRARY) .