#ifndef CONTROLINTERFACE_HPP
#define CONTROLINTERFACE_HPP

namespace EmbeddedMT {
	namespace Backend {
		class ControlInterface {
			public:
				virtual ~ControlInterface() {}
				virtual void moveRight() const = 0;
				virtual void moveLeft() const = 0;
				virtual void moveUp() const = 0;
				virtual void moveDown() const = 0;
		};
	}
}

#endif
