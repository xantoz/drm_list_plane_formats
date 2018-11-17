#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#include <xf86drm.h>
#include <xf86drmMode.h>

#include <drm_fourcc.h>
#include <drm.h>

struct uint_string_map
{
    uint32_t uint;
    char *name;
};

static const char *uint_string_map_find(uint32_t key,
                                        const struct uint_string_map *map,
                                        size_t len)
{
    for (unsigned i = 0; i < len; ++i) {
        if (map[i].uint == key)
            return map[i].name;
    }

    return "NOT FOUND";
}

#define FIND(key, x) (uint_string_map_find((key), (x), sizeof((x))/sizeof((x)[0])))

static const char *fourcc_to_string(uint32_t fourcc)
{
    static const struct uint_string_map formats[] = {
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

    return FIND(fourcc, formats);
}

static const char *encoder_type_to_string(uint32_t encoder_type)
{
    static const struct uint_string_map encoder_types[] = {
        {DRM_MODE_ENCODER_NONE, "DRM_MODE_ENCODER_NONE"},
        {DRM_MODE_ENCODER_DAC, "DRM_MODE_ENCODER_DAC"},
        {DRM_MODE_ENCODER_TMDS, "DRM_MODE_ENCODER_TMDS"},
        {DRM_MODE_ENCODER_LVDS, "DRM_MODE_ENCODER_LVDS"},
        {DRM_MODE_ENCODER_TVDAC, "DRM_MODE_ENCODER_TVDAC"},
        {DRM_MODE_ENCODER_VIRTUAL, "DRM_MODE_ENCODER_VIRTUAL"},
        {DRM_MODE_ENCODER_DSI, "DRM_MODE_ENCODER_DSI"},
        {DRM_MODE_ENCODER_DPMST, "DRM_MODE_ENCODER_DPMST"},
        {DRM_MODE_ENCODER_DPI, "DRM_MODE_ENCODER_DPI"},
    };

    return FIND(encoder_type, encoder_types);
}

static const char *connector_type_to_string(uint32_t connector_type)
{
    static const struct uint_string_map connector_types[] = {
        {DRM_MODE_CONNECTOR_Unknown, "DRM_MODE_CONNECTOR_Unknown"},
        {DRM_MODE_CONNECTOR_VGA, "DRM_MODE_CONNECTOR_VGA"},
        {DRM_MODE_CONNECTOR_DVII, "DRM_MODE_CONNECTOR_DVII"},
        {DRM_MODE_CONNECTOR_DVID, "DRM_MODE_CONNECTOR_DVID"},
        {DRM_MODE_CONNECTOR_DVIA, "DRM_MODE_CONNECTOR_DVIA"},
        {DRM_MODE_CONNECTOR_Composite, "DRM_MODE_CONNECTOR_Composite"},
        {DRM_MODE_CONNECTOR_SVIDEO, "DRM_MODE_CONNECTOR_SVIDEO"},
        {DRM_MODE_CONNECTOR_LVDS, "DRM_MODE_CONNECTOR_LVDS"},
        {DRM_MODE_CONNECTOR_Component, "DRM_MODE_CONNECTOR_Component"},
        {DRM_MODE_CONNECTOR_9PinDIN, "DRM_MODE_CONNECTOR_9PinDIN"},
        {DRM_MODE_CONNECTOR_DisplayPort, "DRM_MODE_CONNECTOR_DisplayPort"},
        {DRM_MODE_CONNECTOR_HDMIA, "DRM_MODE_CONNECTOR_HDMIA"},
        {DRM_MODE_CONNECTOR_HDMIB, "DRM_MODE_CONNECTOR_HDMIB"},
        {DRM_MODE_CONNECTOR_TV, "DRM_MODE_CONNECTOR_TV"},
        {DRM_MODE_CONNECTOR_eDP, "DRM_MODE_CONNECTOR_eDP"},
        {DRM_MODE_CONNECTOR_VIRTUAL, "DRM_MODE_CONNECTOR_VIRTUAL"},
        {DRM_MODE_CONNECTOR_DSI, "DRM_MODE_CONNECTOR_DSI"},
        {DRM_MODE_CONNECTOR_DPI, "DRM_MODE_CONNECTOR_DPI"},
    };

    return FIND(connector_type, connector_types);
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

static const char *drmModeConnection_to_string(drmModeConnection connection)
{
    switch (connection) {
    case DRM_MODE_CONNECTED:         return "DRM_MODE_CONNECTED";
    case DRM_MODE_DISCONNECTED:      return "DRM_MODE_DISCONNECTED";
    case DRM_MODE_UNKNOWNCONNECTION: return "DRM_MODE_UNKNOWNCONNECTION";
    }

    return "???";
}

static const char *drmModeSubPixel_to_string(drmModeSubPixel sp)
{
    switch (sp) {
    case DRM_MODE_SUBPIXEL_UNKNOWN:        return "DRM_MODE_SUBPIXEL_UNKNOWN";
    case DRM_MODE_SUBPIXEL_HORIZONTAL_RGB: return "DRM_MODE_SUBPIXEL_HORIZONTAL_RGB";
    case DRM_MODE_SUBPIXEL_HORIZONTAL_BGR: return "DRM_MODE_SUBPIXEL_HORIZONTAL_BGR";
    case DRM_MODE_SUBPIXEL_VERTICAL_RGB:   return "DRM_MODE_SUBPIXEL_VERTICAL_RGB";
    case DRM_MODE_SUBPIXEL_VERTICAL_BGR:   return "DRM_MODE_SUBPIXEL_VERTICAL_BGR";
    case DRM_MODE_SUBPIXEL_NONE:           return "DRM_MODE_SUBPIXEL_NONE";
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
        if (NULL == drmplane)
            continue;
        drmModeObjectProperties *properties =
            drmModeObjectGetProperties(fd,
                                       drmplane->plane_id,
                                       DRM_MODE_OBJECT_PLANE);
        drmModePropertyPtr prop = NULL;
        uint64_t value = 4000;
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
            char substr[128];
            snprintf(substr, sizeof(substr), "[%s],", plane_type_to_string(value));
            printf("plane %2u (crtc_id: %2u) %-25s format: %#x  %-25s",
                   drmplane->plane_id,
                   drmplane->crtc_id,
                   substr,
                   drmplane->formats[j],
                   fourcc_to_string(drmplane->formats[j]));
            printf("%s", " | possible_crtcs: [");
            for (unsigned k = 0; k < sizeof(uint32_t); ++k) {
                if (drmplane->possible_crtcs & (uint32_t)(1 << k))
                    printf(" %u ", k);
            }
            puts("]");
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

static void list_encoders(int fd)
{
    drmModeRes *res = drmModeGetResources(fd);

    if (res == NULL) {
        fputs("ERROR: Could not get resources\n", stderr);
        abort();
    }

    for (unsigned i = 0; i < res->count_encoders; ++i) {
        drmModeEncoder *enc = drmModeGetEncoder(fd, res->encoders[i]);
        if (NULL == enc)
            continue;

        char substr[128];
        snprintf(substr, sizeof(substr), "[%s],", encoder_type_to_string(enc->encoder_type));
        printf("encoder_id: %2u %-27s crtc_id: %2u",
               enc->encoder_id, substr, enc->crtc_id);
        printf("%s", " | possible_crtcs: [");
        for (unsigned j = 0; j < sizeof(uint32_t); ++j) {
            if (enc->possible_crtcs & (uint32_t)(1 << j))
                printf(" %d ", j);
        }
        putchar(']');

        printf("%s", " | possible_clones: [");
        for (unsigned j = 0; j < sizeof(uint32_t); ++j) {
            if (enc->possible_clones & (uint32_t)(1 << j))
                printf(" %d ", j);
        }
        putchar(']');

        puts("");

        drmModeFreeEncoder(enc);
    }

    if (res)
        drmModeFreeResources(res);
}

static void list_crtcs(int fd)
{
    drmModeRes *res = drmModeGetResources(fd);

    if (res == NULL) {
        fputs("ERROR: Could not get resources\n", stderr);
        abort();
    }

    for (unsigned i = 0; i < res->count_crtcs; ++i) {
        drmModeCrtc *crtc = drmModeGetCrtc(fd, res->crtcs[i]);
        if (NULL == crtc)
            continue;

        char xy[128];
        snprintf(xy, sizeof(xy), "<x,y>=<%u,%u>,", crtc->x, crtc->y);
        char wh[128];
        snprintf(wh, sizeof(wh), "<w,h>=<%u,%u>,", crtc->width, crtc->height);

        printf("crtc_id: %2u, buffer_id: %2u, %-15s %-17s gamma_size: %3d, mode_valid: %d,"
               "mode={\n"
               "    .name: \"%s\",\n"
               "    .clock=%3d,\n"
               "    .hdisplay=%u, .hsync_start=%u, .hsync_end=%u, .htotal=%u, .hskew=%u,\n"
               "    .vdisplay=%u, .vsync_start=%u, .vsync_end=%u, .vtotal=%u, .vscan=%u,\n"
               "    .vrefresh=%u,\n"
               "    .flags=%u,\n"
               "    .type=%u\n"
               "}\n",
               crtc->crtc_id, crtc->buffer_id, xy, wh, crtc->gamma_size, crtc->mode_valid,
               crtc->mode.name,
               crtc->mode.clock,
               crtc->mode.hdisplay, crtc->mode.hsync_start, crtc->mode.hsync_end, crtc->mode.htotal, crtc->mode.hskew,
               crtc->mode.vdisplay, crtc->mode.vsync_start, crtc->mode.vsync_end, crtc->mode.vtotal, crtc->mode.vscan,
               crtc->mode.vrefresh, crtc->mode.flags, crtc->mode.type);

        drmModeFreeCrtc(crtc);
    }

    if (res)
        drmModeFreeResources(res);
}




static void list_connectors(int fd)
{
    drmModeRes *res = drmModeGetResources(fd);

    if (res == NULL) {
        fputs("ERROR: Could not get resources\n", stderr);
        abort();
    }

    for (unsigned i = 0; i < res->count_connectors; ++i) {
        drmModeConnector *conn = drmModeGetConnector(fd, res->connectors[i]);
        if (NULL == conn)
            continue;

        char substr[128];
        snprintf(substr, sizeof(substr), "[%s #%u],",
                 connector_type_to_string(conn->connector_type),
                 conn->connector_type_id);
        char info[128];
        snprintf(info, sizeof(info), "<%ummX%umm>", conn->mmWidth, conn->mmHeight);
        char subpixel[128];
        snprintf(subpixel, sizeof(subpixel), "[%s],", drmModeSubPixel_to_string(conn->subpixel));
        printf("connector_id: %2u %15s %-36s %-35s encoder_id: %2u, connection: %s\n",
               conn->connector_id,
               info,
               substr,
               subpixel,
               conn->encoder_id,
               drmModeConnection_to_string(conn->connection));
        // TODO: modes

        drmModeFreeConnector(conn);
    }

    if (res)
        drmModeFreeResources(res);
}

static void list_caps(int fd)
{
    static const struct uint_string_map caps[] = {
        {DRM_CAP_DUMB_BUFFER, "DRM_CAP_DUMB_BUFFER"},
        {DRM_CAP_VBLANK_HIGH_CRTC, "DRM_CAP_VBLANK_HIGH_CRTC"},
        {DRM_CAP_DUMB_PREFERRED_DEPTH, "DRM_CAP_DUMB_PREFERRED_DEPTH"},
        {DRM_CAP_DUMB_PREFER_SHADOW, "DRM_CAP_DUMB_PREFER_SHADOW"},
        {DRM_CAP_PRIME, "DRM_CAP_PRIME"},
        {DRM_PRIME_CAP_IMPORT, "DRM_PRIME_CAP_IMPORT"},
        {DRM_PRIME_CAP_EXPORT, "DRM_PRIME_CAP_EXPORT"},
        {DRM_CAP_TIMESTAMP_MONOTONIC, "DRM_CAP_TIMESTAMP_MONOTONIC"},
        {DRM_CAP_ASYNC_PAGE_FLIP, "DRM_CAP_ASYNC_PAGE_FLIP"},
        {DRM_CAP_CURSOR_WIDTH, "DRM_CAP_CURSOR_WIDTH"},
        {DRM_CAP_CURSOR_HEIGHT, "DRM_CAP_CURSOR_HEIGHT"},
        {DRM_CAP_ADDFB2_MODIFIERS, "DRM_CAP_ADDFB2_MODIFIERS"},
        {DRM_CAP_PAGE_FLIP_TARGET, "DRM_CAP_PAGE_FLIP_TARGET"},
        {DRM_CAP_CRTC_IN_VBLANK_EVENT, "DRM_CAP_CRTC_IN_VBLANK_EVENT"},
        {DRM_CAP_SYNCOBJ, "DRM_CAP_SYNCOBJ"},
    };

    for (unsigned i = 0; i < sizeof(caps)/sizeof(caps[0]); ++i) {
        uint64_t value;
        drmGetCap(fd, caps[i].uint, &value);
        printf("%s: %zu\n", caps[i].name, value);
    }
}

int main(int argc, char **argv)
{
    const char *card = (argc >= 2) ? argv[1] : "/dev/dri/card0";
    int fd = open(card, O_RDWR | O_CLOEXEC);

    if (fd < 0) {
        fprintf(stderr, "Cannot open card %s: %s\n", card, strerror(errno));
        abort();
    }

    printf("\n--CAPS--\n");
    list_caps(fd);

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

    puts("\n--ENCODERS--");
    list_encoders(fd);

    puts("\n--CONNECTORS--");
    list_connectors(fd);

    puts("\n--CRTCS--");
    list_crtcs(fd);

    return 0;
}

// gcc -o drm_list_plane_formats drm_list_plane_formats.c -ldrm -I/usr/include/libdrm
