#include "Camera.h"
#include "Renderer.h"
#include "Scene.h"

#include <glm/gtc/type_ptr.hpp>
#include <Walnut/Application.h>
#include <Walnut/EntryPoint.h>
#include <Walnut/Image.h>
#include <Walnut/Timer.h>

class ExampleLayer : public Walnut::Layer
{
public:
	ExampleLayer() : m_camera(45.0f, 0.01f, 1000.0f)
	{
		Sphere sphere;
		sphere.Albedo = glm::vec3{ 1.0f, 0.0f, 1.0f };
		sphere.Position = glm::vec3{ 0.0f, 0.0f, 0.0f };
		sphere.Radius = 0.5f;
		m_scene.Spaeres.emplace_back(std::move(sphere));

		sphere.Albedo = glm::vec3{ 0.0f, 1.0f, 0.0f };
		sphere.Position = glm::vec3{ 1.0f, 1.0f, -5.0f };
		sphere.Radius = 2.0f;
		m_scene.Spaeres.emplace_back(std::move(sphere));
	}

	virtual void OnUpdate(float ts) override
	{
		m_camera.OnUpdate(ts);
	}

	virtual void OnUIRender() override
	{
		ImGui::Begin("Settings");
		ImGui::Text("Last Frame: %.3fms", m_lastFrameTime);
		if (ImGui::Button("Render"))
		{
			Render();
		}
		ImGui::End();

		ImGui::Begin("Scene");
		for (size_t i = 0; i < m_scene.Spaeres.size(); ++i)
		{
			ImGui::PushID(i);
			Sphere &sphere = m_scene.Spaeres[i];
			ImGui::DragFloat3("Position", glm::value_ptr(sphere.Position), 0.1f);
			ImGui::DragFloat("Radius", &sphere.Radius, 0.1f);
			ImGui::ColorEdit3("Albedo", glm::value_ptr(sphere.Albedo));
			ImGui::Separator();
			ImGui::PopID();
		}
		ImGui::End();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("View Port");
		m_viewportWidth = ImGui::GetContentRegionAvail().x;
		m_viewportHeight = ImGui::GetContentRegionAvail().y;
		if (auto pImage = m_renderer.GetFinalImage(); pImage)
		{
			ImGui::Image(pImage->GetDescriptorSet(),
				ImVec2{ (float)pImage->GetWidth(), (float)pImage->GetHeight() },
				ImVec2{ 0.0f, 1.0f }, ImVec2{ 1.0f, 0.0f });
		}
		ImGui::End();
		ImGui::PopStyleVar();

		Render();
	}

	void Render()
	{

		Walnut::Timer timer;

		m_camera.OnResize(m_viewportWidth, m_viewportHeight);

		m_renderer.OnResize(m_viewportWidth, m_viewportHeight);
		m_renderer.Render(m_scene, m_camera);

		m_lastFrameTime = timer.ElapsedMillis();
	}

private:
	float m_lastFrameTime = 0.0f;

	uint32_t m_viewportWidth = 0;
	uint32_t m_viewportHeight = 0;

	Camera m_camera;
	Renderer m_renderer;
	Scene m_scene;
};

Walnut::Application* Walnut::CreateApplication(int argc, char** argv)
{
	Walnut::ApplicationSpecification spec;
	spec.Name = "Ray Tracing";

	Walnut::Application* app = new Walnut::Application(spec);
	app->PushLayer<ExampleLayer>();
	app->SetMenubarCallback([app]()
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Exit"))
			{
				app->Close();
			}
			ImGui::EndMenu();
		}
	});

	return app;
}
