#ifndef DCEL_PHYSICS_INTEGRATOR_H
#define DCEL_PHYSICS_INTEGRATOR_H

#include "vector3.h"

struct Integrator {
	float value;
	float vel;
	float accel;
	float force;
	float mass;

	float damping;
	float attraction;
	bool targeting;
	float _target;

	Integrator() {
		damping = 0.5f;
		attraction = 0.2f;
		mass = 1;
	}
	Integrator(float value) {
		damping = 0.5f;
		attraction = 0.2f;
		mass = 1;
		this->value = value;
	}


	Integrator(float value, float damping, float attraction) {
		mass = 1;
		this->value = value;
		this->damping = damping;
		this->attraction = attraction;
	}


	void set(float v) {
		value = v;
	}


	void update() {
		if (targeting) {
			force += attraction * (_target - value);      
		}

		accel = force / mass;
		vel = (vel + accel) * damping;
		value += vel;

		force = 0;
	}


	void set_target(float t) {
		targeting = true;
		_target = t;
	}


	void noTarget() {
		targeting = false;
	}
};

class integrator_vec3f
{
private:
	vector3f vvalue;
	vector3f vtarget;
	Integrator vintergrator[3];
public:
	integrator_vec3f(){}
	void set_value(vector3f v){
		vvalue=v;
		vintergrator[0].value=v.x();
		vintergrator[1].value=v.y();
		vintergrator[2].value=v.z();
	}
	vector3f value(){return vvalue;}
	void set_target(vector3f t){
		vtarget=t;
		vintergrator[0].set_target(t.x());
		vintergrator[1].set_target(t.y());
		vintergrator[2].set_target(t.z());
	}	
	vector3f target(){return vtarget;}
	void update(){

		vintergrator[0].update();
		vintergrator[1].update();
		vintergrator[2].update();

		vvalue.set_x(vintergrator[0].value); 
		vvalue.set_y(vintergrator[1].value); 
		vvalue.set_z(vintergrator[2].value); 
	}
	bool actived(){
		if ((vvalue-vtarget).norm()<0.001f)
		{
			return false;
		}
		return true;
	}
};

#endif