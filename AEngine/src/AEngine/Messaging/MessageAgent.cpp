/**
 * \file
 * \author Christien Alden (34119981)
*/
#include "MessageAgent.h"
#include "MessageServiceImpl.h"

namespace AEngine
{
	MessageAgent::MessageAgent(Agent agent, MessageServiceImpl &service)
		: m_identifier(agent), m_service(service)
	{

	}

	void MessageAgent::Destroy()
	{
		m_service.DestroyAgent(m_identifier);
	}

	void MessageAgent::AddToCategory(AgentCategory category)
	{
		m_service.AddAgentToCategory(m_identifier, category);
	}

	void MessageAgent::AddToCategory(AgentCategorySet categories)
	{
		m_service.AddAgentToCategory(m_identifier, categories);
	}

	void MessageAgent::RemoveFromCategory(AgentCategory category)
	{
		m_service.RemoveAgentFromCategory(m_identifier, category);
	}

	void MessageAgent::RemoveFromCategory(AgentCategorySet categories)
	{
		m_service.RemoveAgentFromCategory(m_identifier, categories);
	}

	void MessageAgent::RegisterMessageHandler(MessageType type, MessageCallback callback)
	{
		m_service.RegisterMessageHandler(m_identifier, type, callback);
	}

	void MessageAgent::UnregisterMessageHandler(MessageType type)
	{
		m_service.UnregisterMessageHandler(m_identifier, type);
	}

	void MessageAgent::BroadcastMessage(MessageType type, MessageData payload)
	{
		m_service.SendMessageToAllAgents(m_identifier, type, payload);
	}

	void MessageAgent::SendMessageToAgent(Agent to, MessageType type, MessageData payload)
	{
		m_service.SendMessageToAgent(m_identifier, to, type, payload);
	}

	void MessageAgent::SendMessageToAgent(AgentSet to, MessageType type, MessageData payload)
	{
		m_service.SendMessageToAgent(m_identifier, to, type, payload);
	}

	void MessageAgent::SendMessageToCategory(AgentCategory to, MessageType type, MessageData payload)
	{
		m_service.SendMessageToCategory(m_identifier, to, type, payload);
	}

	void MessageAgent::SendMessageToCategory(AgentCategorySet to, MessageType type, MessageData payload)
	{
		m_service.SendMessageToCategory(m_identifier, to, type, payload);
	}

	const AgentCategorySet MessageAgent::GetRegisteredCategories() const
	{
		return m_service.GetRegisteredAgentCategories(m_identifier);
	}

	const MessageTypeSet MessageAgent::GetRegisteredMessageTypes() const
	{
		return m_service.GetRegisteredMessageTypes(m_identifier);
	}
}
