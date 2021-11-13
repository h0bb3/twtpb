// This code is provided by Tobias Olsson under a CC BY 4.0 licence https://creativecommons.org/licenses/by/4.0/
#include "StdAfx.h"
#include ".\view.h"


namespace view {

View::View(const model::Model &a_model, ViewCore &a_core) : m_model(a_model), m_core(a_core){
}

View::~View(void) {
}

}
