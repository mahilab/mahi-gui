// MIT License
//
// Copyright (c) 2020 Mechatronics and Haptic Interfaces Lab - Rice University
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// Author(s): Evan Pezent (epezent@rice.edu)

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <Mahi/Gui.hpp>
#include <Mahi/Util.hpp>

using namespace mahi::gui;
using namespace mahi::util;

// this example is ported from the official GLFW examples

static void gear(float inner_radius, float outer_radius, float width, int teeth,
                 float tooth_depth) {
    int   i;
    float r0, r1, r2;
    float angle, da;
    float u, v, len;

    r0 = inner_radius;
    r1 = outer_radius - tooth_depth / 2.f;
    r2 = outer_radius + tooth_depth / 2.f;

    da = 2.f * (float)PI / teeth / 4.f;

    glShadeModel(GL_FLAT);

    glNormal3f(0.f, 0.f, 1.f);

    /* draw front face */
    glBegin(GL_QUAD_STRIP);
    for (i = 0; i <= teeth; i++) {
        angle = i * 2.f * (float)PI / teeth;
        glVertex3f(r0 * (float)cos(angle), r0 * (float)sin(angle), width * 0.5f);
        glVertex3f(r1 * (float)cos(angle), r1 * (float)sin(angle), width * 0.5f);
        if (i < teeth) {
            glVertex3f(r0 * (float)cos(angle), r0 * (float)sin(angle), width * 0.5f);
            glVertex3f(r1 * (float)cos(angle + 3 * da), r1 * (float)sin(angle + 3 * da),
                       width * 0.5f);
        }
    }
    glEnd();

    /* draw front sides of teeth */
    glBegin(GL_QUADS);
    da = 2.f * (float)PI / teeth / 4.f;
    for (i = 0; i < teeth; i++) {
        angle = i * 2.f * (float)PI / teeth;

        glVertex3f(r1 * (float)cos(angle), r1 * (float)sin(angle), width * 0.5f);
        glVertex3f(r2 * (float)cos(angle + da), r2 * (float)sin(angle + da), width * 0.5f);
        glVertex3f(r2 * (float)cos(angle + 2 * da), r2 * (float)sin(angle + 2 * da), width * 0.5f);
        glVertex3f(r1 * (float)cos(angle + 3 * da), r1 * (float)sin(angle + 3 * da), width * 0.5f);
    }
    glEnd();

    glNormal3f(0.0, 0.0, -1.0);

    /* draw back face */
    glBegin(GL_QUAD_STRIP);
    for (i = 0; i <= teeth; i++) {
        angle = i * 2.f * (float)PI / teeth;
        glVertex3f(r1 * (float)cos(angle), r1 * (float)sin(angle), -width * 0.5f);
        glVertex3f(r0 * (float)cos(angle), r0 * (float)sin(angle), -width * 0.5f);
        if (i < teeth) {
            glVertex3f(r1 * (float)cos(angle + 3 * da), r1 * (float)sin(angle + 3 * da),
                       -width * 0.5f);
            glVertex3f(r0 * (float)cos(angle), r0 * (float)sin(angle), -width * 0.5f);
        }
    }
    glEnd();

    /* draw back sides of teeth */
    glBegin(GL_QUADS);
    da = 2.f * (float)PI / teeth / 4.f;
    for (i = 0; i < teeth; i++) {
        angle = i * 2.f * (float)PI / teeth;

        glVertex3f(r1 * (float)cos(angle + 3 * da), r1 * (float)sin(angle + 3 * da), -width * 0.5f);
        glVertex3f(r2 * (float)cos(angle + 2 * da), r2 * (float)sin(angle + 2 * da), -width * 0.5f);
        glVertex3f(r2 * (float)cos(angle + da), r2 * (float)sin(angle + da), -width * 0.5f);
        glVertex3f(r1 * (float)cos(angle), r1 * (float)sin(angle), -width * 0.5f);
    }
    glEnd();

    /* draw outward faces of teeth */
    glBegin(GL_QUAD_STRIP);
    for (i = 0; i < teeth; i++) {
        angle = i * 2.f * (float)PI / teeth;

        glVertex3f(r1 * (float)cos(angle), r1 * (float)sin(angle), width * 0.5f);
        glVertex3f(r1 * (float)cos(angle), r1 * (float)sin(angle), -width * 0.5f);
        u   = r2 * (float)cos(angle + da) - r1 * (float)cos(angle);
        v   = r2 * (float)sin(angle + da) - r1 * (float)sin(angle);
        len = (float)sqrt(u * u + v * v);
        u /= len;
        v /= len;
        glNormal3f(v, -u, 0.0);
        glVertex3f(r2 * (float)cos(angle + da), r2 * (float)sin(angle + da), width * 0.5f);
        glVertex3f(r2 * (float)cos(angle + da), r2 * (float)sin(angle + da), -width * 0.5f);
        glNormal3f((float)cos(angle), (float)sin(angle), 0.f);
        glVertex3f(r2 * (float)cos(angle + 2 * da), r2 * (float)sin(angle + 2 * da), width * 0.5f);
        glVertex3f(r2 * (float)cos(angle + 2 * da), r2 * (float)sin(angle + 2 * da), -width * 0.5f);
        u = r1 * (float)cos(angle + 3 * da) - r2 * (float)cos(angle + 2 * da);
        v = r1 * (float)sin(angle + 3 * da) - r2 * (float)sin(angle + 2 * da);
        glNormal3f(v, -u, 0.f);
        glVertex3f(r1 * (float)cos(angle + 3 * da), r1 * (float)sin(angle + 3 * da), width * 0.5f);
        glVertex3f(r1 * (float)cos(angle + 3 * da), r1 * (float)sin(angle + 3 * da), -width * 0.5f);
        glNormal3f((float)cos(angle), (float)sin(angle), 0.f);
    }

    glVertex3f(r1 * (float)cos(0), r1 * (float)sin(0), width * 0.5f);
    glVertex3f(r1 * (float)cos(0), r1 * (float)sin(0), -width * 0.5f);

    glEnd();

    glShadeModel(GL_SMOOTH);

    /* draw inside radius cylinder */
    glBegin(GL_QUAD_STRIP);
    for (i = 0; i <= teeth; i++) {
        angle = i * 2.f * (float)PI / teeth;
        glNormal3f(-(float)cos(angle), -(float)sin(angle), 0.f);
        glVertex3f(r0 * (float)cos(angle), r0 * (float)sin(angle), -width * 0.5f);
        glVertex3f(r0 * (float)cos(angle), r0 * (float)sin(angle), width * 0.5f);
    }
    glEnd();
}

class GearsDemo : public Application {
public:
    GearsDemo(Application::Config conf) : Application(conf) {

        static float pos[4] = {5.f, 5.f, 10.f, 0.f};

        glLightfv(GL_LIGHT0, GL_POSITION, pos);
        glEnable(GL_CULL_FACE);
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        glEnable(GL_DEPTH_TEST);

        /* make the gears */
        gear1 = glGenLists(1);
        glNewList(gear1, GL_COMPILE);
        glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, &Reds::Crimson.r);
        gear(1.f, 4.f, 1.f, 20, 0.7f);
        glEndList();

        gear2 = glGenLists(1);
        glNewList(gear2, GL_COMPILE);
        glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, &Greens::Chartreuse.r);
        gear(0.5f, 2.f, 2.f, 10, 0.7f);
        glEndList();

        gear3 = glGenLists(1);
        glNewList(gear3, GL_COMPILE);
        glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, &Blues::DeepSkyBlue.r);
        gear(1.3f, 2.f, 0.5f, 10, 0.7f);
        glEndList();

        glEnable(GL_NORMALIZE);

        reshape(300, 300);
        on_window_resized.connect(this, &GearsDemo::reshape);
        on_keyboard.connect(this, &GearsDemo::key);
    }

    void update() {
        ImGui::Begin("Gears");
        ImGui::DragFloat("Rot. X", &view_rotx, 1, 0, 360);
        ImGui::DragFloat("Rot. Y", &view_roty, 1, 0, 360);
        ImGui::DragFloat("Rot. Z", &view_rotz, 1, 0, 360);
        ImGui::DragFloat("Speed", &speed, 1, 0, 1000);
        ImGui::End();
        // animate
        angle = speed * (float)time().as_seconds();
    }

    void draw() override {
        glPushMatrix();
        glRotatef(view_rotx, 1.0, 0.0, 0.0);
        glRotatef(view_roty, 0.0, 1.0, 0.0);
        glRotatef(view_rotz, 0.0, 0.0, 1.0);
        glPushMatrix();
        glTranslatef(-3.0, -2.0, 0.0);
        glRotatef(angle, 0.0, 0.0, 1.0);
        glCallList(gear1);
        glPopMatrix();
        glPushMatrix();
        glTranslatef(3.1f, -2.f, 0.f);
        glRotatef(-2.f * angle - 9.f, 0.f, 0.f, 1.f);
        glCallList(gear2);
        glPopMatrix();
        glPushMatrix();
        glTranslatef(-3.1f, 4.2f, 0.f);
        glRotatef(-2.f * angle - 25.f, 0.f, 0.f, 1.f);
        glCallList(gear3);
        glPopMatrix();
        glPopMatrix();
    }

    void reshape(int width, int height) {
        float h = (float)height / (float)width;
        float xmax, znear, zfar;
        znear = 5.0f;
        zfar  = 30.0f;
        xmax  = znear * 0.5f;
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glFrustum(-xmax, xmax, -xmax * h, xmax * h, znear, zfar);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glTranslatef(0.0, 0.0, -20.0);
    }

    void key(int k, int s, int action, int mods) {
        if (action != GLFW_PRESS)
            return;
        switch (k) {
            case GLFW_KEY_Z:
                if (mods & GLFW_MOD_SHIFT)
                    view_rotz -= 5.0;
                else
                    view_rotz += 5.0;
                break;
            case GLFW_KEY_ESCAPE: glfwSetWindowShouldClose(m_window, GLFW_TRUE); break;
            case GLFW_KEY_UP: view_rotx += 5.0; break;
            case GLFW_KEY_DOWN: view_rotx -= 5.0; break;
            case GLFW_KEY_LEFT: view_roty += 5.0; break;
            case GLFW_KEY_RIGHT: view_roty -= 5.0; break;
            default: return;
        }
    }

    float view_rotx = 0.f, view_roty = 0.f, view_rotz = 0.f;
    int   gear1, gear2, gear3;
    float angle = 0.f;
    float speed = 100.0f;
};

int main(int argc, char const* argv[]) {\

    Application::Config conf;
    conf.transparent = true;
    conf.decorated = false;
    conf.msaa = 4;
    conf.gl_forward_compat = false;
    GearsDemo demo(conf);
    demo.run();
    return 0;
}
