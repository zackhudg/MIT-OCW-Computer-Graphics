#include "curve.h"
#include "extra.h"
#ifdef WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>
using namespace glm;
using namespace std;

namespace
{
    // Approximately equal to.  We don't want to use == because of
    // precision issues with floating point.
    inline bool approx(const vec3& lhs, const vec3& rhs)
    {
        const float eps = 1e-8f;
        return length(lhs - rhs) < eps;
    }


}


Curve evalBezier(const vector< vec3 >& P, unsigned steps)
{
    // Check
    int numCurves = 0;
    if (P.size() < 4 || P.size() % 3 != 1)
    {
        cerr << "evalBezier must be called with 3n+1 control points." << endl;
        exit(0);
    }
    else {
        numCurves = (P.size() - 1) / 3;
    }

    // TODO:
    // You should implement this function so that it returns a Curve
    // (e.g., a vector< CurvePoint >).  The variable "steps" tells you
    // the number of points to generate on each piece of the spline.
    // At least, that's how the sample solution is implemented and how
    // the SWP files are written.  But you are free to interpret this
    // variable however you want, so long as you can control the
    // "resolution" of the discretized spline curve with it.

    // Make sure that this function computes all the appropriate
    // Vector3fs for each CurvePoint: V,T,N,B.
    // [NBT] should be unit and orthogonal.

    // Also note that you may assume that all Bezier curves that you
    // receive have G1 continuity.  Otherwise, the TNB will not be
    // be defined at points where this does not hold.

    cerr << "\t>>> evalBezier has been called with the following input:" << endl;

    bool dim2 = true;
    cerr << "\t>>> Control points (type vector< vec3 >): " << endl;
    for (unsigned i = 0; i < P.size(); ++i)
    {
        cerr << "\t>>> " << to_string(P[i]) << endl;
        if (dim2 && P[i].z != 0.0f) dim2 = false;
    }

    cerr << "\t>>> Steps (type steps): " << steps << endl;
    cerr << "\t>>> Returning empty curve." << endl;
    
    // Preallocate a curve with steps+1 CurvePoints
    Curve curve(1 + numCurves * steps);
    vec3 a, b, c, d;
    for (int curveCounter = 0; curveCounter < numCurves; curveCounter++){
        // Fill it in counterclockwise
        // SLOW: could get address of P[0] then just add 1 every time before defining b,c,d, and a=d at end. Not sure if faster or not.
        int PIndexOffset = curveCounter * 3;
        a = P[PIndexOffset];
        b = P[PIndexOffset+1];
        c = P[PIndexOffset+2];
        d = P[PIndexOffset+3];
        for (unsigned i = 0; i <= steps; ++i)
        {
            // step from 0 to 1
            float t = float(i) / steps;

            // 2D first
            // Initialize position
            float Vx = (((d.x + 3 * (b.x - c.x) - a.x) * t + 3 * (a.x - 2 * b.x + c.x)) * t + 3 * (b.x - a.x)) * t + a.x;
            float Vy = (((d.y + 3 * (b.y - c.y) - a.y) * t + 3 * (a.y - 2 * b.y + c.y)) * t + 3 * (b.y - a.y)) * t + a.y;

            // Tangent (unit) vector is first derivative
            float Tx = 3 * (((d.x + 3 * (b.x - c.x) - a.x) * t + 2 * (a.x - 2 * b.x + c.x)) * t + b.x - a.x);
            float Ty = 3 * (((d.y + 3 * (b.y - c.y) - a.y) * t + 2 * (a.y - 2 * b.y + c.y)) * t + b.y - a.y);
            
            float Vz, Tz;

            if (dim2) {
                float Vz, Tz = 0;
                curve[i].V = vec3(Vx, Vy, Vz);
                curve[i].T = vec3(Tx, Ty, Tz);
                curve[i].T = curve[i].T / length(curve[i].T);

                // Binormal is facing up.
                curve[i].B = vec3(0, 0, 1);

                // Normal (unit) vector is B x T
                curve[i].N = cross(curve[i].B, curve[i].T);
            }
            else {
                Vz = (((d.z + 3 * (b.z - c.z) - a.z) * t + 3 * (a.z - 2 * b.z + c.z)) * t + 3 * (b.z - a.z)) * t + a.z;
                Tz = 3 * (((d.z + 3 * (b.z - c.z) - a.z) * t + 2 * (a.z - 2 * b.z + c.z)) * t + b.z - a.z);
                curve[i].V = vec3(Vx, Vy, Vz);
                curve[i].T = vec3(Tx, Ty, Tz);
                curve[i].T = curve[i].T / length(curve[i].T);
                
                // Normal (unit) vector is second derivative. Used in 3d
                float Nx = 6 * ((d.x + 3 * (b.x - c.x) - a.x) * t + a.x - 2 * b.x + c.x);
                float Ny = 6 * ((d.y + 3 * (b.y - c.y) - a.y) * t + a.y - 2 * b.y + c.y);
                float Nz = 6 * ((d.z + 3 * (b.z - c.z) - a.z) * t + a.z - 2 * b.z + c.z);
                curve[i].N = vec3(Vx, Vy, Vz);
                curve[i].N = curve[i].N / length(curve[i].N);

                //incremental B
            }

        }
    }


    return curve;
}

Curve evalBspline( const vector< vec3 >& P, unsigned steps )
{
    // Check
    if( P.size() < 4 )
    {
        cerr << "evalBspline must be called with 4 or more control points." << endl;
        exit( 0 );
    }

    // TODO:
    // It is suggested that you implement this function by changing
    // basis from B-spline to Bezier.  That way, you can just call
    // your evalBezier function.

    cerr << "\t>>> evalBSpline has been called with the following input:" << endl;

    cerr << "\t>>> Control points (type vector< vec3 >): "<< endl;
    for( unsigned i = 0; i < P.size(); ++i )
    {
        cerr << "\t>>> " << to_string(P[i]) << endl;
    }

    cerr << "\t>>> Steps (type steps): " << steps << endl;
    cerr << "\t>>> Returning empty curve." << endl;

    // Return an empty curve right now.
    // Preallocate a curve with steps+1 CurvePoints
    Curve curve(steps + 1);

    // Fill it in counterclockwise
    for (unsigned i = 0; i <= steps; ++i)
    {
        // step from 0 to 1
        float t = float(i) / steps;

        // Initialize position
        // We're pivoting counterclockwise around the y-axis
        float x = (((d.x() + 3 * (b.x() - c.x()) - a.x()) * t + 3 * (a.x() - 2 * b.x() + c.x())) * t + 3 * (b.x() - a.x()))* t + a.x();
        float y = (((d.y() + 3 * (b.y() - c.y()) - a.y()) * t + 3 * (a.y() - 2 * b.y() + c.y())) * t + 3 * (b.y() - a.y()))* t + a.y();
        
        curve[i].V = vec3(x, y, )

        // Tangent vector is first derivative
        curve[i].T = vec3(-sin(t), cos(t), 0);

        // Normal vector is second derivative
        curve[i].N = vec3(-cos(t), -sin(t), 0);

        // Finally, binormal is facing up.
        curve[i].B = vec3(0, 0, 1);
    }

    return curve;
}

Curve evalCircle( float radius, unsigned steps )
{
    // This is a sample function on how to properly initialize a Curve
    // (which is a vector< CurvePoint >).
    
    // Preallocate a curve with steps+1 CurvePoints
    Curve R( steps+1 );

    // Fill it in counterclockwise
    for( unsigned i = 0; i <= steps; ++i )
    {
        // step from 0 to 2pi
        float t = 2.0f * M_PI * float( i ) / steps;

        // Initialize position
        // We're pivoting counterclockwise around the y-axis
        R[i].V = radius * vec3( cos(t), sin(t), 0 );
        
        // Tangent vector is first derivative
        R[i].T = vec3( -sin(t), cos(t), 0 );
        
        // Normal vector is second derivative
        R[i].N = vec3( -cos(t), -sin(t), 0 );

        // Finally, binormal is facing up.
        R[i].B = vec3( 0, 0, 1 );
    }

    return R;
}

void drawCurve( const Curve& curve, float framesize )
{
    // Save current state of OpenGL
    glPushAttrib( GL_ALL_ATTRIB_BITS );

    // Setup for line drawing
    glDisable( GL_LIGHTING ); 
    glColor4f( 1, 1, 1, 1 );
    glLineWidth( 1 );
    
    // Draw curve
    glBegin( GL_LINE_STRIP );
    for( unsigned i = 0; i < curve.size(); ++i )
    {
        glVertex( curve[ i ].V );
    }
    glEnd();

    glLineWidth( 1 );

    // Draw coordinate frames if framesize nonzero
    if( framesize != 0.0f )
    {
        mat4 M;

        for( unsigned i = 0; i < curve.size(); ++i )
        {
            M = mat4(vec4(curve[i].N, 0.0f), vec4(curve[i].B, 0.0f), vec4(curve[i].T, 0.0f), vec4(curve[i].V, 0.0f));
            /*M.setCol( 0, vec4( curve[i].N, 0 ) );
            M.setCol( 1, vec4( curve[i].B, 0 ) );
            M.setCol( 2, vec4( curve[i].T, 0 ) );
            M.setCol( 3, vec4( curve[i].V, 1 ) );*/

            glPushMatrix();
            glMultMatrixf( value_ptr(M) );
            glScaled( framesize, framesize, framesize );
            glBegin( GL_LINES );
            glColor3f( 1, 0, 0 ); glVertex3d( 0, 0, 0 ); glVertex3d( 1, 0, 0 );
            glColor3f( 0, 1, 0 ); glVertex3d( 0, 0, 0 ); glVertex3d( 0, 1, 0 );
            glColor3f( 0, 0, 1 ); glVertex3d( 0, 0, 0 ); glVertex3d( 0, 0, 1 );
            glEnd();
            glPopMatrix();
        }
    }
    
    // Pop state
    glPopAttrib();
}

