#include <bits/types/__sigset_t.h>
#include <setjmp.h> /* Get `__jmp_buf'.  */
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define png_voidcast(type, value) (value)

typedef struct __jmp_buf_tag jmp_buf[1];
typedef unsigned int png_uint_32;
typedef unsigned char png_byte;

// incomplete
struct png_struct_def {
    jmp_buf jmp_buf_local;
};

// incomplete
struct png_info_def {
    /* The following are necessary for every PNG file */
    png_uint_32 width; /* width of image in pixels (from IHDR) */
};

typedef struct png_struct_def png_struct;
typedef const png_struct *png_const_structp;
typedef png_struct *png_structp;
typedef void *png_voidp;
typedef const void *png_const_voidp;
typedef png_byte *png_bytep;
typedef const png_byte *png_const_bytep;
typedef struct png_info_def png_info;
typedef png_info *png_infop;
typedef const png_info *png_const_infop;

typedef struct png_control {
    png_structp png_ptr;
    png_infop info_ptr;
    png_voidp error_buf; /* Always a jmp_buf at present. */

    png_const_bytep memory; /* Memory buffer. */
    size_t size;            /* Size of the memory buffer. */

    unsigned int for_write : 1;  /* Otherwise it is a read structure */
    unsigned int owned_file : 1; /* We own the file in io_ptr */
} png_control;

typedef struct png_control *png_controlp;

typedef struct {
    png_controlp opaque; /* Initialize to NULL, free with png_image_free */
    png_uint_32 version; /* Set to PNG_IMAGE_VERSION */
    png_uint_32 width;   /* Image width in pixels (columns) */
    png_uint_32 height;  /* Image height in pixels (rows) */
    png_uint_32 format;  /* Image format as defined below */
    png_uint_32 flags;   /* A bit mask containing informational flags */
    png_uint_32 colormap_entries;
    /* Number of entries in the color-map */

    /* In the event of an error or warning the following field will be set to a
     * non-zero value and the 'message' field will contain a '\0' terminated
     * string with the libpng error or warning message.  If both warnings and
     * an error were encountered, only the error is recorded.  If there
     * are multiple warnings, only the first one is recorded.
     *
     * The upper 30 bits of this value are reserved, the low two bits contain
     * a value as follows:
     */
#define PNG_IMAGE_WARNING 1
#define PNG_IMAGE_ERROR 2
    /*
     * The result is a two-bit code such that a value more than 1 indicates
     * a failure in the API just called:
     *
     *    0 - no warning or error
     *    1 - warning
     *    2 - error
     *    3 - error preceded by warning
     */
#define PNG_IMAGE_FAILED(png_cntrl) ((((png_cntrl).warning_or_error) & 0x03) > 1)

    png_uint_32 warning_or_error;

    char message[64];
} png_image, *png_imagep;

// first parameter changed from png_const_structrp to png_const_structp
void png_free_default(png_const_structp png_ptr, png_voidp ptr __attribute__((annotate("psr.source")))) {
    if (png_ptr == NULL || ptr == NULL)
        return;
    // #endif /* USER_MEM */

    free(ptr);
}

// first parameter changed from png_const_structrp to png_const_structp
void png_free(png_const_structp png_ptr, png_voidp ptr) {
    if (png_ptr == NULL || ptr == NULL)
        return;

    // #ifdef PNG_USER_MEM_SUPPORTED
    //    if (png_ptr->free_fn != NULL)
    //       png_ptr->free_fn(png_constcast(png_structp,png_ptr), ptr);

    //    else
    png_free_default(png_ptr, ptr);
}

static int png_image_free_function(png_voidp argument) {
    png_imagep image = png_voidcast(png_imagep, argument);
    png_controlp cp = image->opaque;
    png_control c;

    /* Double check that we have a png_ptr - it should be impossible to get here
     * without one.
     */
    if (cp->png_ptr == NULL)
        return 0;

        /* First free any data held in the control structure. */
#ifdef PNG_STDIO_SUPPORTED
    if (cp->owned_file != 0) {
        FILE *fp = png_voidcast(FILE *, cp->png_ptr->io_ptr);
        cp->owned_file = 0;

        /* Ignore errors here. */
        if (fp != NULL) {
            cp->png_ptr->io_ptr = NULL;
            (void)fclose(fp);
        }
    }
#endif

    /* Copy the control structure so that the original, allocated, version can be
     * safely freed.  Notice that a png_error here stops the remainder of the
     * cleanup, but this is probably fine because that would indicate bad memory
     * problems anyway.
     */
    c = *cp;
    image->opaque = &c;
    png_free(c.png_ptr, cp);

    /* Then the structures, calling the correct API. */
    // These functions are generated using PNG_FUNTION macros. In the whole IR this is properly translated. For this
    // example we omit this
    //    if (c.for_write != 0)
    //    {
    // #     ifdef PNG_SIMPLIFIED_WRITE_SUPPORTED
    //          png_destroy_write_struct(&c.png_ptr, &c.info_ptr);
    // #     else
    //          png_error(c.png_ptr, "simplified write not supported");
    // #     endif
    //    }
    //    else
    //    {
    // #     ifdef PNG_SIMPLIFIED_READ_SUPPORTED
    //          png_destroy_read_struct(&c.png_ptr, &c.info_ptr, NULL);
    // #     else
    //          png_error(c.png_ptr, "simplified read not supported");
    // #     endif
    //    }

    /* Success. */
    return 1;
}

void png_image_free(png_imagep image) {
    /* Safely call the real function, but only if doing so is safe at this point
     * (if not inside an error handling context).  Otherwise assume
     * png_safe_execute will call this API after the return.
     */
    if (image != NULL && image->opaque != NULL && image->opaque->error_buf == NULL) {
        /* Ignore errors here: */
        (void)png_safe_execute(image, NULL, image);
        image->opaque = NULL;
    }
}

int /* PRIVATE */
png_safe_execute(png_imagep image_in, int (*function)(png_voidp), png_voidp arg) {
    volatile png_imagep image = image_in;
    volatile int result;
    volatile png_voidp saved_error_buf;
    jmp_buf safe_jmpbuf;

    /* Safely execute function(arg) with png_error returning to this function. */
    saved_error_buf = image->opaque->error_buf;
    result = setjmp(safe_jmpbuf) == 0;

    if (result != 0) {

        image->opaque->error_buf = safe_jmpbuf;
        result = function(image);
    }

    image->opaque->error_buf = saved_error_buf;

    /* And do the cleanup prior to any failure return. */
    if (result == 0)
        png_image_free(image);

    return result;
}

int main() {
    unsigned char data[] = {
        0x89, 0x50, 0x4e, 0x47, 0xd, 0xa, 0x1a, 0xa,  0x0,  0x0,  0x0,  0xd,  0x49, 0x48, 0x44,
        0x52, 0x0,  0x0,  0x0,  0x0, 0x0, 0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x2e,
        0x90, 0x68, 0xf,  0x0,  0x0, 0x0, 0x0,  0x49, 0x45, 0x4e, 0x44, 0xae, 0x42, 0x60, 0x82,
    };
    png_imagep image = (png_imagep)malloc(sizeof(png_image));
    return png_safe_execute(image, png_image_free_function, image);
    ;
}