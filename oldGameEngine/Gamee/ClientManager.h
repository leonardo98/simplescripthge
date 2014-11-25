#ifndef MYENGINE_CLIENTMANAGER_H
#define MYENGINE_CLIENTMANAGER_H

#include "Client.h"

class ClientManager {
public:
	~ClientManager();
	ClientManager();
	void Draw();
	void Update(float dt);
	void OnMouseMove(const FPoint2D &mousePos);
	void OnMouseDown(const FPoint2D &mousePos);
private:
	const int MAX;
	const int FIRST;
	const int STEP;
	typedef std::list<Client *> Clients;
	Client *_activeClient;
	Clients _clients;
	float _newClientTimeCounter;
	int _clientCounter;

	Clients _sellers;
};

#endif//MYENGINE_CLIENTMANAGER_H