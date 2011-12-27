#include "../Core/Core.h"
#include "../Core/Math.h"
#include "ClientManager.h"
#include "AnnaPers.h"

ClientManager::~ClientManager() {
	for (Clients::iterator i = _clients.begin(), e = _clients.end(); i != e; ++i) {
		delete (*i);
	}
}

ClientManager::ClientManager() 
: _newClientTimeCounter(0.01f)
, MAX(4)
, FIRST(275)
, STEP(150)
, _activeClient(NULL)
, _clientCounter(0)
{
}

void ClientManager::OnMouseDown(const FPoint2D &mousePos) {
	if (_activeClient->Seller()) {

	} else if (_activeClient != NULL && _activeClient->IsWaitProduct() && _activeClient->_productWant == AnnaPers::GetProductType()) {
		AnnaPers::DropProduct();
		_activeClient->SetProduct();
		_newClientTimeCounter = Math::random(0.1f, 0.3f);
	}
}

void ClientManager::OnMouseMove(const FPoint2D &mousePos) {
	Client *old = _activeClient;
	_activeClient = NULL;
	for (Clients::iterator i = _clients.begin(), e = _clients.end(); i != e; ++i) {
		if ((*i)->IsWaitProduct() && (*i)->PixelCheck(mousePos)) {
			_activeClient = (*i);
			break;
		}
	}
	for (Clients::iterator i = _sellers.begin(), e = _sellers.end(); i != e; ++i) {
		if ((*i)->IsWaitProduct() && (*i)->PixelCheck(mousePos)) {
			_activeClient = (*i);
			break;
		}
	}
	if (_activeClient != old) {
		if (old) {
			old->SetActive(false);
		}
		if (_activeClient) {
			_activeClient->SetActive(true);
		}
	}
}

void ClientManager::Draw() {
	for (Clients::iterator i = _clients.begin(), e = _clients.end(); i != e; ++i) {
		if (_activeClient == (*i)) {
			float b = 2;
			Render::SetBlendMode(3);
			Render::PushMatrix();
			Render::MatrixMove(-b, 0);
			(*i)->Draw();
			Render::MatrixMove(2 * b, 0);
			(*i)->Draw();
			Render::MatrixMove(-b, b);
			(*i)->Draw();
			Render::MatrixMove(0, -2 * b);
			(*i)->Draw();
			Render::PopMatrix();
			Render::SetBlendMode(BLEND_DEFAULT);
		}
		(*i)->Draw();
	}
	for (Clients::iterator i = _sellers.begin(), e = _sellers.end(); i != e; ++i) {
		if (_activeClient == (*i)) {
			float b = 2;
			Render::SetBlendMode(3);
			Render::PushMatrix();
			Render::MatrixMove(-b, 0);
			(*i)->Draw();
			Render::MatrixMove(2 * b, 0);
			(*i)->Draw();
			Render::MatrixMove(-b, b);
			(*i)->Draw();
			Render::MatrixMove(0, -2 * b);
			(*i)->Draw();
			Render::PopMatrix();
			Render::SetBlendMode(BLEND_DEFAULT);
		}
		(*i)->Draw();
	}
}

void ClientManager::Update(float dt) {
	for (Clients::iterator i = _sellers.begin(); i != _sellers.end(); ) {
		(*i)->Update(dt);
		if ((*i)->GetState() == state_finish) {
			delete (*i);
			i = _sellers.erase(i);
		} else {
			++i;
		}
	}
	std::vector<Client *> freePosition(MAX, (Client *)NULL);
	for (Clients::iterator i = _clients.begin(); i != _clients.end(); ) {
		(*i)->Update(dt);
		int screenPosition = ((*i)->GetPos() - FIRST) / STEP;
		if ((*i)->GetState() == state_finish) {
			delete (*i);
			i = _clients.erase(i);
		} else if ((*i)->GetState() != state_go_out) {
			assert(0 <= screenPosition && screenPosition < MAX);
			assert(freePosition[screenPosition] == NULL);
			freePosition[screenPosition] = *i;
			++i;
		} else {
			++i;
		}
	}
	if (_newClientTimeCounter >= 0.f) {
		_newClientTimeCounter -= dt;
		if (_newClientTimeCounter < 0.f) {
			if (_clientCounter < 4) {
				_newClientTimeCounter = Math::random(2.f, 4.f);
			} else {
				if (_sellers.size() == 0) {
					Client *s = new Client(FIRST + STEP * 4);
					s->CreateSeller("", 100);
					_sellers.push_back(s);
				}
				_newClientTimeCounter = Math::random(0.1f, 0.3f);
			}
			for (int i = 0; i < MAX; ++i) {
				if (freePosition[i] == NULL) {
					float p = FIRST + i * STEP;
					if (i == MAX - 1) {
						_clients.push_front(new Client(p));
						++_clientCounter;
					} else {
						int j = i + 1;
						for (; j < MAX && freePosition[j] == NULL; ++j);
						if (j == MAX) {
							_clients.push_front(new Client(p));
							++_clientCounter;
						} else {
							freePosition[j]->SetPos(p);
							_newClientTimeCounter = Math::random(0.1f, 0.3f);
						}
					}
					return;
				}
			}
		}
	}
}