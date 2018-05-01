#include "Scene/MainBase.h"
#include "Scene/SubBase.h"
#include "Scene/Transform.h"

#include "Physics/SphereCollider.h"

#include "SceneManager.h"

MainBase::MainBase(GameObject* gameObject)
    : Component(gameObject),
    transform_(gameObject->createComponent<Transform>()),
    initialised_(false)
{

}

void MainBase::update(float deltaTime)
{
    std::vector<SubBase*> bases = SceneManager::instance()->findAllComponentsInScene<SubBase>();
    if (bases.size() != 0)
    {
        initialised_ = true;
    }

    if (initialised_ == true)
    {
        if (bases.size() == 0)
        {
            std::cout << "Done" << std::endl;
            expose();
        }
    }
}

void MainBase::expose()
{
    GameObject* shield = transform_->children()[0]->gameObject();
    delete shield;
}