#include "Object.h"

////////////////////////////////////////////////////////////////////////

void Object::UpdateEvents() {
	
}

void Object::Update(const float dt) {
	m_time += dt;
	
}

void Object::Draw() {
	if(!GetRenderer()){ return; }
	
	SDL_SetRenderDrawColor(GetRenderer(), m_color.r, m_color.g, m_color.b, m_color.a);
	
	SDL_RenderFillRect(GetRenderer(), &m_rect);
}

////////////////////////////////////////////////////////////////////////

void Object::SetPos(const float inX, const float inY) {
	m_rect.x = (int)inX;
	m_rect.y = (int)inY;
}

void Object::SetSize(float inW, float inH) {
    m_rect.w = inW;
    m_rect.h = inH;
    m_init_rect = m_rect;
}



















