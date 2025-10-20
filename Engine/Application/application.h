#pragma once


namespace Application
{
	class Application
	{
	public:
		/**
		 * Highest level of abstraction class to run the application
		 */
		void Run();
	private:
		void Init();
		void MainLoop();
		void Shutdown();

	};
}
