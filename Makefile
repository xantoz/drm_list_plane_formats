all: drm_list_plane_formats

clean:
	rm -f drm_list_plane_formats

drm_list_plane_formats: drm_list_plane_formats.c
	gcc -o drm_list_plane_formats drm_list_plane_formats.c -ldrm -I/usr/include/libdrm
