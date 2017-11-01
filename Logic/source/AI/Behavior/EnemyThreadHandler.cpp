#include <AI\Behavior\EnemyThreadHandler.h>
#include <AI\EntityManager.h>
#include <AI\Enemy.h>
#include <AI\Behavior\AStar.h>
#include <AI\Behavior\Behavior.h>

#include <Engine\Profiler.h>

using namespace Logic;

EnemyThreadHandler::EnemyThreadHandler()
{
    m_killChildren = false;

    resetThreads();
    initThreads();
}

void EnemyThreadHandler::initThreads()
{
    for (std::thread *&t : threads)
        t = newd std::thread(&EnemyThreadHandler::threadMain, this);
}

EnemyThreadHandler::~EnemyThreadHandler()
{
    deleteThreads();
}

void EnemyThreadHandler::resetThreads()
{
    for (std::thread *&t : threads)
        t = nullptr;
}

void EnemyThreadHandler::deleteThreads()
{
    m_killChildren = true;
    for (std::thread *t : threads)
    {
        if (t)
        {
            if (t->joinable())
                t->join();
            delete t;
        }
    }
}

void EnemyThreadHandler::updateEnemiesAndPath(WorkData &data)
{
    AStar &aStar = AStar::singleton();
    aStar.loadTargetIndex(data.player);

    std::vector<const DirectX::SimpleMath::Vector3*> path = aStar.getPath(data.index);
    const std::vector<Enemy*> &enemies = data.manager->getAliveEnemies()[data.index];

    for (size_t i = 0; i < enemies.size(); i++) // (!) size can change throughout the loop (!)
        enemies[i]->getBehavior()->getPath().setPath(path); // TODO: enemy->setPath
}

void EnemyThreadHandler::threadMain()
{
    while (!m_killChildren)
    {
        std::lock_guard<std::mutex> lock(m_workMutex);
        if (!m_work.empty())
        {
            WorkData todo = m_work.front();
            m_work.pop();
            updateEnemiesAndPath(todo);
        }
        std::this_thread::sleep_for(2ns);
    }
}

void EnemyThreadHandler::addWork(WorkData data)
{
    m_work.push(data);
}