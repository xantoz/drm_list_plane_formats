#include <stdlib.h>
#include <stdio.h>

#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#include <xf86drm.h>
#include <xf86drmMode.h>

#include <drm_fourcc.h>

static const char *fourcc_to_string(uint32_t fourcc)
{
    static const struct {
        uint32_t format;
        char *name;
    } formats[] = {
        {DRM_FORMAT_C8, "DRM_FORMAT_C8"},
        {DRM_FORMAT_GR88, "DRM_FORMAT_GR88"},
        {DRM_FORMAT_RGB332, "DRM_FORMAT_RGB332"},
        {DRM_FORMAT_BGR233, "DRM_FORMAT_BGR233"},
        {DRM_FORMAT_XRGB4444, "DRM_FORMAT_XRGB4444"},
        {DRM_FORMAT_XBGR4444, "DRM_FORMAT_XBGR4444"},
        {DRM_FORMAT_RGBX4444, "DRM_FORMAT_RGBX4444"},
        {DRM_FORMAT_BGRX4444, "DRM_FORMAT_BGRX4444"},
        {DRM_FORMAT_ARGB4444, "DRM_FORMAT_ARGB4444"},
        {DRM_FORMAT_ABGR4444, "DRM_FORMAT_ABGR4444"},
        {DRM_FORMAT_RGBA4444, "DRM_FORMAT_RGBA4444"},
        {DRM_FORMAT_BGRA4444, "DRM_FORMAT_BGRA4444"},
        {DRM_FORMAT_XRGB1555, "DRM_FORMAT_XRGB1555"},
        {DRM_FORMAT_XBGR1555, "DRM_FORMAT_XBGR1555"},
        {DRM_FORMAT_RGBX5551, "DRM_FORMAT_RGBX5551"},
        {DRM_FORMAT_BGRX5551, "DRM_FORMAT_BGRX5551"},
        {DRM_FORMAT_ARGB1555, "DRM_FORMAT_ARGB1555"},
        {DRM_FORMAT_ABGR1555, "DRM_FORMAT_ABGR1555"},
        {DRM_FORMAT_RGBA5551, "DRM_FORMAT_RGBA5551"},
        {DRM_FORMAT_BGRA5551, "DRM_FORMAT_BGRA5551"},
        {DRM_FORMAT_RGB565, "DRM_FORMAT_RGB565"},
        {DRM_FORMAT_BGR565, "DRM_FORMAT_BGR565"},
        {DRM_FORMAT_RGB888, "DRM_FORMAT_RGB888"},
        {DRM_FORMAT_BGR888, "DRM_FORMAT_BGR888"},
        {DRM_FORMAT_XRGB8888, "DRM_FORMAT_XRGB8888"},
        {DRM_FORMAT_XBGR8888, "DRM_FORMAT_XBGR8888"},
        {DRM_FORMAT_RGBX8888, "DRM_FORMAT_RGBX8888"},
        {DRM_FORMAT_BGRX8888, "DRM_FORMAT_BGRX8888"},
        {DRM_FORMAT_ARGB8888, "DRM_FORMAT_ARGB8888"},
        {DRM_FORMAT_ABGR8888, "DRM_FORMAT_ABGR8888"},
        {DRM_FORMAT_RGBA8888, "DRM_FORMAT_RGBA8888"},
        {DRM_FORMAT_BGRA8888, "DRM_FORMAT_BGRA8888"},
        {DRM_FORMAT_XRGB2101010, "DRM_FORMAT_XRGB2101010"},
        {DRM_FORMAT_XBGR2101010, "DRM_FORMAT_XBGR2101010"},
        {DRM_FORMAT_RGBX1010102, "DRM_FORMAT_RGBX1010102"},
        {DRM_FORMAT_BGRX1010102, "DRM_FORMAT_BGRX1010102"},
        {DRM_FORMAT_ARGB2101010, "DRM_FORMAT_ARGB2101010"},
        {DRM_FORMAT_ABGR2101010, "DRM_FORMAT_ABGR2101010"},
        {DRM_FORMAT_RGBA1010102, "DRM_FORMAT_RGBA1010102"},
        {DRM_FORMAT_BGRA1010102, "DRM_FORMAT_BGRA1010102"},
        {DRM_FORMAT_YUYV, "DRM_FORMAT_YUYV"},
        {DRM_FORMAT_YVYU, "DRM_FORMAT_YVYU"},
        {DRM_FORMAT_UYVY, "DRM_FORMAT_UYVY"},
        {DRM_FORMAT_VYUY, "DRM_FORMAT_VYUY"},
        {DRM_FORMAT_AYUV, "DRM_FORMAT_AYUV"},
        {DRM_FORMAT_NV12, "DRM_FORMAT_NV12"},
        {DRM_FORMAT_NV21, "DRM_FORMAT_NV21"},
        {DRM_FORMAT_NV16, "DRM_FORMAT_NV16"},
        {DRM_FORMAT_NV61, "DRM_FORMAT_NV61"},
        {DRM_FORMAT_YUV410, "DRM_FORMAT_YUV410"},
        {DRM_FORMAT_YVU410, "DRM_FORMAT_YVU410"},
        {DRM_FORMAT_YUV411, "DRM_FORMAT_YUV411"},
        {DRM_FORMAT_YVU411, "DRM_FORMAT_YVU411"},
        {DRM_FORMAT_YUV420, "DRM_FORMAT_YUV420"},
        {DRM_FORMAT_YVU420, "DRM_FORMAT_YVU420"},
        {DRM_FORMAT_YUV422, "DRM_FORMAT_YUV422"},
        {DRM_FORMAT_YVU422, "DRM_FORMAT_YVU422"},
        {DRM_FORMAT_YUV444, "DRM_FORMAT_YUV444"},
        {DRM_FORMAT_YVU444, "DRM_FORMAT_YVU444"},
    };

    for (unsigned i = 0; i < sizeof(formats)/sizeof(formats[0]); ++i) {
        if (formats[i].format == fourcc)
            return formats[i].name;
    }

    return "NOT FOUND";
}

static const char *plane_type_to_string(uint32_t plane_type)
{
    switch (plane_type) {
        case DRM_PLANE_TYPE_OVERLAY:  return "DRM_PLANE_TYPE_OVERLAY";
        case DRM_PLANE_TYPE_PRIMARY:  return "DRM_PLANE_TYPE_PRIMARY";
        case DRM_PLANE_TYPE_CURSOR:   return "DRM_PLANE_TYPE_CURSOR";
    }

    return "???";
}

static void list_planes(int fd)
{
    drmModePlaneRes *plane_res = drmModeGetPlaneResources(fd);

    if (plane_res == NULL) {
        fputs("ERROR: Could not get plane resources\n", stderr);
        abort();
    }

    for (unsigned int i = 0; i < plane_res->count_planes; i++) {
        drmModePlane *drmplane = drmModeGetPlane(fd, plane_res->planes[i]);
        drmModeObjectProperties *properties =
            drmModeObjectGetProperties(fd,
                                       drmplane->plane_id,
                                       DRM_MODE_OBJECT_PLANE);
        drmModePropertyPtr prop = NULL;
        uint64_t value = 0;
        for (unsigned j = 0; j < properties->count_props; ++j) {
            prop = drmModeGetProperty(fd, properties->props[j]);
            if (0 == strcasecmp("TYPE", prop->name)) {
                value = properties->prop_values[j];
                break;
            }
            drmModeFreeProperty(prop);
            prop = NULL;
        }
        for (unsigned j = 0; j < drmplane->count_formats; ++j) {
            printf("plane %d [%s] format: %#x %s\n",
                   drmplane->plane_id,
                   plane_type_to_string(value),
                   drmplane->formats[j],
                   fourcc_to_string(drmplane->formats[j]));
        }
        drmModeFreeProperty(prop);
        prop = NULL;
        drmModeFreeObjectProperties(properties);
        properties = NULL;
        drmModeFreePlane(drmplane);
        drmplane = NULL;
    }

    if (plane_res)
        drmModeFreePlaneResources(plane_res);
}

int main()
{
    const char *card = "/dev/dri/card0";
    int fd = open(card, O_RDWR | O_CLOEXEC);

    if (fd < 0) {
        fprintf(stderr, "Cannot open card %s: %s\n", card, strerror(errno));
        abort();
    }

    puts("\n--WITHOUT UNIVERSAL PLANES--");
    list_planes(fd);

    // Universal planes allows accessing all the planes (including primary)
    if (drmSetClientCap(fd, DRM_CLIENT_CAP_UNIVERSAL_PLANES, 1)) {
        fputs("ERROR: Failed to set Universal planes capability\n", stderr);
        abort();
    }

    if (drmSetClientCap(fd, DRM_CLIENT_CAP_ATOMIC, 1)) {
        fputs("ERROR: No DRM Atomic support found\n", stderr);
        abort();
    }

    puts("\n--WITH UNIVERSAL PLANES--");
    list_planes(fd);

    return 0;
}

// gcc -o drm_list_plane_formats drm_list_plane_formats.c -ldrm -I/usr/include/libdrm
