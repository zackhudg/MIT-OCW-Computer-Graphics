// Arcball camera by Eugene Hsu
// Based on 6.839 sample code for rotation code.
// Extended to handle translation (MIDDLE) and scale (RIGHT)

// -*-c++-*-
#ifndef CAMERA_H
#define CAMERA_H

//#include glm

class Camera
{
public:

    Camera();
    
    typedef enum { NONE, LEFT, MIDDLE, RIGHT } Button;

    // You must call all of the Set*() functions before you use this!
    // I didn't put it into the constructor because it's inconvenient
    // to initialize stuff in my opengl application.
    
    void SetDimensions(int w, int h);
    void SetViewport(int x, int y, int w, int h);
    void SetPerspective(float fovy);

    // Call from whatever UI toolkit
    void MouseClick(Button button, int x, int y);
    void MouseDrag(int x, int y);
    void MouseRelease(int x, int y);

    // Apply viewport, perspective, and modeling
    // use these instead of 
    void ApplyViewport() const;
    void ApplyPerspective() const;
    void ApplyModelview() const;

    // Set for relevant vars
    void SetCenter(const vec3& center);
    void SetRotation(const mat4& rotation);
    void SetDistance(const float distance);

    // Get for relevant vars
    vec3 GetCenter() const { return mCurrentCenter; }
    mat4 GetRotation() const { return mCurrentRot; }
    float GetDistance() const { return mCurrentDistance; }
    
private:

    // States 
    int     mDimensions[2];
    int     mStartClick[2];
    Button  mButtonState;

    // For rotation
    mat4 mStartRot;
    mat4 mCurrentRot;

    // For translation
    float   mPerspective[2];
    int     mViewport[4];
    vec3 mStartCenter;
    vec3 mCurrentCenter;

    // For zoom
    float   mStartDistance;
    float   mCurrentDistance;

    void ArcBallRotation(int x, int y);
    void PlaneTranslation(int x, int y);
    void DistanceZoom(int x, int y);
};

#endif
