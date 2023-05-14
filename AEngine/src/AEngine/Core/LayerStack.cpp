#include "LayerStack.h"

namespace AEngine
{
	LayerStack::~LayerStack()
	{
		Clear();
	}

	void LayerStack::Clear()
	{
		for (auto it = m_layers.begin(); it != m_layers.end(); ++it)
		{
			(*it)->OnDetach();
		}
		m_layers.clear();
	}

    bool LayerStack::RemoveLayer(const std::string &ident)
    {
		for (auto it = m_layers.begin(); it != m_layers.end(); ++it)
		{
			if ((*it)->Ident() == ident)
			{
				(*it)->OnDetach();
				m_layers.erase(it);
				return true;
			}
		}

		return false;
    }
	
    void LayerStack::PushLayer(std::unique_ptr<Layer> layer)
	{
		m_layers.push_back(std::move(layer));
		m_layers.back()->OnAttach();
	}

	LayerStack::stack::const_iterator LayerStack::begin()
	{
		return m_layers.begin();
	}

	LayerStack::stack::const_iterator LayerStack::end()
	{
		return m_layers.end();
	}
}