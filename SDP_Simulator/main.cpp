#include "FEHLCD.h"
#include "FEHImages.h"
#include "header_files/renderer.h"
#include "header_files/menu.h"

int main()
{
    // test object
    Container* container = new Container();

    // (*container).objects.backgroundColor = {140, 170, 200};
    // (*container).objects.cameraPosition = {0, 100, 0};
    // Object* object = new Object();
    // object->center = {0, 100, 200};
    // object->color = {230, 230, 240};
    // object->reflectionValue = 0.5;
    // object->refractionValue = 15;
    
    // object->vertices.push_back({100, 200, 400}); // 0: top-right
    // object->vertices.push_back({100, 0, 400}); // 1: bottom-right
    // object->vertices.push_back({-100, 200, 400}); // 2: top-left
    // object->vertices.push_back({-100, 0, 200}); // 3: bottom-left
    // object->vertices.push_back({0, 1000, 1500}); // 4: up/back
    // object->vertices.push_back({100, 800, 800}); // 5: top-right-back
    
    // object->faces.push_back({0, 1, 2}); // top-right triangle
    // object->faceColors.push_back({230, 230, 240});
    // object->faces.push_back({2, 1, 3}); // bottom-left triangle
    // object->faceColors.push_back({230, 230, 240});
    // object->faces.push_back({2, 4, 0}); // edge case test
    // object->faceColors.push_back({230, 230, 240});
    // object->faces.push_back({2, 5, 1}); // z buffer test
    // object->faceColors.push_back({230, 230, 240});

    // container->objects.platforms.push_back(*object);

    // render(*container);

    while (1) {
        drawMenu(*container);

        float x, y;

        if (LCD.Touch(&x, &y)) {
            while (LCD.Touch(&x, &y)); // makes sure it doesn't spam between menu and other screens
            if (x >= 195 && x <= 305) {
                if (y >= 20 && y <= 65) {
                    stageSelect(*container);
                }
                /*
                Add more options here
                */
            }
        }

        LCD.Update();
    }
    return 0;
}