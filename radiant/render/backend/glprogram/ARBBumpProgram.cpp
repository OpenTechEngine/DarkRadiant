#include "ARBBumpProgram.h"

namespace render
{

namespace
{
    /* CONSTANT FRAGMENT PROGRAM PARAMETERS
     * These should match what is used by interaction_fp.cg
     */
    const int C2_LIGHT_ORIGIN = 2;
    const int C3_LIGHT_COLOR = 3;
    const int C4_VIEW_ORIGIN = 4;
    const int C6_LIGHT_SCALE = 6;
    const int C7_AMBIENT_FACTOR = 7;

}

void ARBBumpProgram::applyRenderParams(const Vector3& viewer, 
                                       const Matrix4& localToWorld, 
                                       const Vector3& origin, 
                                       const Vector3& colour, 
                                       const Matrix4& world2light,
                                       float ambientFactor)
{
    Matrix4 world2local(localToWorld);
    matrix4_affine_invert(world2local);

    // Calculate the light origin in object space
    Vector3 localLight(origin);
    matrix4_transform_point(world2local, localLight);

    // Viewer location in object space
    Vector3 localViewer(viewer);
    matrix4_transform_point(world2local, localViewer);

    Matrix4 local2light(world2light);
    matrix4_multiply_by_matrix4(local2light, localToWorld); // local->world->light

    // view origin
    glProgramLocalParameter4fARB(
        GL_FRAGMENT_PROGRAM_ARB, C4_VIEW_ORIGIN, localViewer.x(), localViewer.y(), localViewer.z(), 0
    );

    // light origin
    glProgramLocalParameter4fARB(
        GL_FRAGMENT_PROGRAM_ARB, C2_LIGHT_ORIGIN, localLight.x(), localLight.y(), localLight.z(), 1
    );

    // light colour
    glProgramLocalParameter4fARB(
        GL_FRAGMENT_PROGRAM_ARB, C3_LIGHT_COLOR, colour.x(), colour.y(), colour.z(), 0
    );

	// light scale
	glProgramLocalParameter4fARB(
        GL_FRAGMENT_PROGRAM_ARB, C6_LIGHT_SCALE, _lightScale, _lightScale, _lightScale, 0
    );

	// ambient factor
	glProgramLocalParameter4fARB(
        GL_FRAGMENT_PROGRAM_ARB, C7_AMBIENT_FACTOR, ambientFactor, 0, 0, 0
    );

    glActiveTexture(GL_TEXTURE3);
    glClientActiveTexture(GL_TEXTURE3);

    glMatrixMode(GL_TEXTURE);
    glLoadMatrixd(local2light);
    glMatrixMode(GL_MODELVIEW);

    GlobalOpenGL_debugAssertNoErrors();
}

}



