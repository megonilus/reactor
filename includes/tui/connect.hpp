// connect with backend
#pragma once

#include <cstdint>
#include <memory>
#include <sched.h>
#include <string_view>
#include <system_error>
namespace tui
{

	// Abstract interface for connecting with backend
	// implementations : IPC, TCP
	class Connect
	{
	public:
		Connect(const Connect&)			   = default;
		Connect(Connect&&)				   = delete;
		Connect& operator=(const Connect&) = default;
		Connect& operator=(Connect&&)	   = delete;
		virtual ~Connect()				   = default;

		// use string_view instead of string
		// because string_view is optimized RO string
		virtual std::error_code connect(std::string_view endpoint) = 0;

		virtual void disconnect() noexcept = 0;

		virtual std::error_code send_command(std::string_view cmd) = 0;

		virtual std::error_code request_state() = 0;

		virtual void set_state_callback() noexcept = 0;

		[[nodiscard]] virtual bool is_connected() const noexcept = 0;

		static void get_default_values();
	};

	using ConnectPtr = std::unique_ptr<Connect>;
	// for now it's enough
} // namespace tui
