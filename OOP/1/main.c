//
// Created by ed on 07.03.17.
//

#include <stdio.h>
#include <stdbool.h>

#include <GL/glut.h>

#include "core/commands.h"
#include "core/execute.h"
#include "core/canvas.h"
#include "core/errors.h"

#include "point.h"

const unsigned int WINDOW_HEIGHT = 500;

const unsigned int WINDOW_WIDTH = 500;

point_t mouse_position = {-1, -1};

bool mouse_button_left_pressed = false;

bool mouse_button_right_pressed = false;

const char* error_name(error_t error) {
	const char* msg = NULL;
	switch (error) {
		case NONE:
			msg = "";
			break;
		case OUT_OF_MEMORY:
			msg = "out of memory";
			break;
		case NOT_INITIALIZED:
			msg = "not initialized";
			break;
		case ALREADY_INITIALIZED:
			msg = "already initialized";
			break;
		case MODEL_NOT_LOADED:
			msg = "model not loaded";
			break;
		case UNABLE_TO_OPEN_FILE:
			msg = "unable to open file";
			break;
		case INVALID_FILE:
			msg = "invalid file";
			break;
		case UNKNOWN_COMMAND:
			msg = "unknown";
			break;
	}
	return msg;
}

error_t log_if_error(const char* action, error_t error) {
	if (error != NONE) {
		fprintf(stderr, "Error on %s: %s.\n", action, error_name(error));
	}
	return error;
}

void draw_line(const vector_t* a, const vector_t* b) {
	glVertex2d(a->x, a->y);
	glVertex2d(b->x, b->y);
}

void redraw(void) {
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	glColor3f(0.0, 0.4, 0.2);
	glPointSize(3.0);

	glBegin(GL_LINES);
	command_data_t command_data;
	command_data.render_data.canvas.height = WINDOW_WIDTH;
	command_data.render_data.canvas.width = WINDOW_HEIGHT;
	command_data.render_data.canvas.draw_line = draw_line;
	log_if_error("rendering", execute(RENDER, &command_data));

	glEnd();
	glFlush();
}

void mouse_left_handler(int state, int x, int y) {
	switch (state) {
		case GLUT_UP:
			mouse_button_left_pressed = false;
			break;
		case GLUT_DOWN:
			mouse_button_left_pressed = true;
			break;
		default:
			break;
	}
}

void mouse_right_handler(int state, int x, int y) {
	switch (state) {
		case GLUT_UP:
			mouse_button_right_pressed = false;
			break;
		case GLUT_DOWN:
			mouse_button_right_pressed = true;
			break;
		default:
			break;
	}
}

void mouse_wheel_handler(int state, int x, int y) {
	const double factor = 0.9;
	bool ok = true;
	command_data_t command_data;
	switch (state) {
		case GLUT_DOWN:
			command_data.transform_object_data.transformation.uniform_scaling.factor = factor;
			break;
		case GLUT_UP:
			command_data.transform_object_data.transformation.uniform_scaling.factor = 1.0 / factor;
			break;
		default:
			ok = false;
			break;
	}
	if (ok) {
		command_data.transform_object_data.transformation.type = UNIFORM_SCALING;
		log_if_error("object transformation", execute(TRANSFORM_OBJECT, &command_data));
		redraw();
	}
}

void mouse_button_handler(int button, int state, int x, int y) {
	switch (button) {
		case GLUT_LEFT_BUTTON:
			mouse_left_handler(state, x, y);
			break;
		case GLUT_RIGHT_BUTTON:
			mouse_right_handler(state, x, y);
			break;
		case 3:
			if (state != GLUT_UP) {
				mouse_wheel_handler(GLUT_UP, x, y);
			}
			break;
		case 4:
			if (state != GLUT_UP) {
				mouse_wheel_handler(GLUT_DOWN, x, y);
			}
			break;
		default:
			break;
	}
	glutPostRedisplay();
}

void mouse_passive_move_handler(int x, int y) {
	mouse_position.y = y;
	mouse_position.x = x;
}

void mouse_move_handler(int x, int y) {
	if (mouse_position.x >= 0 && mouse_position.y >= 0) {
		int dx = x - mouse_position.x;
		int dy = y - mouse_position.y;

		if (dx || dy && (mouse_button_right_pressed || mouse_button_left_pressed)) {
			command_data_t command_data;
			if (mouse_button_left_pressed) {
				command_data.transform_object_data.transformation.type = ROTATION;
				command_data.transform_object_data.transformation.rotation.axis_x = -dy;
				command_data.transform_object_data.transformation.rotation.axis_y = dx;
				command_data.transform_object_data.transformation.rotation.axis_z = 0.0;
				command_data.transform_object_data.transformation.rotation.angle = to_radians(1.5);
				log_if_error("object transformation", execute(TRANSFORM_OBJECT, &command_data));
			} else if (mouse_button_right_pressed) {
				command_data.transform_camera_data.transformation.type = TRANSLATION;
				command_data.transform_camera_data.transformation.translation.displacement_x = -dx * 0.01;
				command_data.transform_camera_data.transformation.translation.displacement_y = -dy * 0.01;
				command_data.transform_camera_data.transformation.translation.displacement_z = 0.0;
				log_if_error("camera transformation", execute(TRANSFORM_CAMERA, &command_data));
			}
			redraw();
		}
	}

	mouse_passive_move_handler(x, y);
}

error_t init_core() {
	error_t error = NONE;

	command_data_t command_data;
	command_data.transform_camera_data.transformation.type = TRANSLATION;
	command_data.transform_camera_data.transformation.translation.displacement_x = 1.5;
	command_data.transform_camera_data.transformation.translation.displacement_y = -1.5;
	command_data.transform_camera_data.transformation.translation.displacement_z = 10.0;
	error = log_if_error("camera transformation", execute(TRANSFORM_CAMERA, &command_data));

	return error;
}

error_t load_core(const char* file_path) {
	command_data_t load_command_data;
	load_command_data.load_model_data.file_path = file_path;

	log_if_error("model loading", execute(LOAD_MODEL, &load_command_data));

	command_data_t command_data;
	command_data.transform_object_data.transformation.type = ROTATION;
	command_data.transform_object_data.transformation.rotation.axis_x = 1;
	command_data.transform_object_data.transformation.rotation.axis_y = 0;
	command_data.transform_object_data.transformation.rotation.axis_z = 0;
	command_data.transform_object_data.transformation.rotation.angle = to_radians(70);
	log_if_error("object rotation", execute(TRANSFORM_OBJECT, &command_data));

	return NONE;
}

int main(int argc, char** argv) {
	if (argc == 1) {
		printf("Usage:\n");
		printf("\t3dView file_name\n");
		printf("\n");
		printf("Example:\n");
		printf("\t3dView cube.txt\n");
		return -1;
	}

	// glut initialization
	glutInit(&argc, argv);
	glutInitWindowPosition(10, 10);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);

	// create glut window
	glutCreateWindow("3D Viewer");
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glMatrixMode(GL_PROJECTION);
	gluOrtho2D(0.0, 300.0, 0.0, 300.0);

	if (init_core() == NONE) {
		if (load_core(argv[1]) == NONE) {
			glutDisplayFunc(redraw);
			glutMouseFunc(mouse_button_handler);
			glutPassiveMotionFunc(mouse_passive_move_handler);
			glutMotionFunc(mouse_move_handler);
		}
	}

	glutMainLoop();

	command_data_t shutdown_command_data;
	log_if_error("shutdown", execute(SHUTDOWN, &shutdown_command_data));

	return 0;
}
