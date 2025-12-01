#include "FEHLCD.h"
#include "data.h"
#include "renderer.h"

int main()
{
    // test object
    Container* container = new Container();

    (*container).objects.backgroundColor = {100, 100, 100};
    (*container).objects.cameraPosition = {0, 100, 0};
    Object* object = new Object();
    object->center = {0, 100, 200};
    object->color = {230, 230, 230};
    object->reflectionValue = 0.5;
    object->refractionValue = 10;

    // Vertices of a rectangle in 3D space
    object->vertices.push_back({100, 200, 400});  // 0: top-right
    object->vertices.push_back({100, 0, 400});    // 1: bottom-right
    object->vertices.push_back({-100, 200, 400}); // 2: top-left
    object->vertices.push_back({-100, 0, 600});   // 3: bottom-left
    object->vertices.push_back({-0, 1000, 400});   // 4: up

    // Only two triangles to cover the rectangle
    object->faces.push_back({0, 1, 2});  // top-right triangle
    object->faceColors.push_back({230, 230, 230});
    object->faces.push_back({2, 1, 3});  // bottom-left triangle
    object->faceColors.push_back({230, 230, 230});
    object->faces.push_back({2, 4, 0});  // edge case test
    object->faceColors.push_back({230, 230, 230});

    container->objects.platforms.push_back(*object);


    render(*container);

    while (1) {
        LCD.Update();
        // Never end
    }
    return 0;
}