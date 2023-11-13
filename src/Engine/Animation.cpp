#include "Animation.h"

#include "AssetLoader.h"
#include "ETime.h"

namespace MyEngine {

	Animation::Animation(Vector3 position, Vector3 scale, Color color) :
		Shape({ position, scale, {0,0,0}, {0, 0, 0, 0}, {1,0,0}, {0,1,0}, {0,0,1}, color }) {
		spriteSheetID = 0;
		currentFrame = 0;
		amountFrames = 0;

		frameWidth = 0.0f;
		frameHeight = 0.0f;

		elapsedTime = 0.00;
		durationInSecs = 0.00;

		mirrorX = 1;
		mirrorY = 1;
		
		setVertex();
	}

	Animation::~Animation() {
		AssetLoader::unloadImage(spriteSheetID);
	}

	void Animation::setVertex() {
		transform.color.normalize();
		vertex.push_back({ 1, 1, 0 });
		vertex.push_back({ 1, 0, 0 });
		vertex.push_back({ 0, 0, 0 });
		vertex.push_back({ 0, 1, 0 });
		alignVertex();
	}

	void Animation::update()
	{
		elapsedTime += ETime::getDeltaTime();

		double normalizedTime = fmod(elapsedTime, durationInSecs);
		currentFrame = static_cast<int>(normalizedTime / durationInSecs * amountFrames) * mirrorX;
	}

	void Animation::setSpriteSheet(unsigned int spriteSheetID, int amountColumns, int amountRows, double durationInSecs) {
		this->spriteSheetID = spriteSheetID;

		// Set the rest of the variables
		frameWidth = 1.0f / amountColumns;
		frameHeight = 1.0f / amountRows;
		amountFrames = amountColumns * amountRows;
		this->durationInSecs = durationInSecs;
	}

	void Animation::setFrame(int frame) {
		if (frame > 0 && frame < amountFrames) {
			currentFrame = frame;
		}
		else {
			cout << "Frame out of bounds!\n";
		}
	}

	void Animation::setMirrorX(bool mirrorX) {
		mirrorX ? this->mirrorX = -1 : this->mirrorX = 1;
	}

	void Animation::setMirrorY(bool mirrorY) {
		mirrorY ? this->mirrorY = -1 : this->mirrorY = 1;
	}

	void Animation::draw() {
		float vertexData[] = {
			vertex[0].x, vertex[0].y, vertex[0].z, /**/ transform.color.r, transform.color.g, transform.color.b, /**/ mirrorX * frameWidth * (currentFrame + 1.0f), mirrorY * 1.0f,
			vertex[1].x, vertex[1].y, vertex[1].z, /**/ transform.color.r, transform.color.g, transform.color.b, /**/ mirrorX * frameWidth * (currentFrame + 1.0f), mirrorY * 0.0f,
			vertex[2].x, vertex[2].y, vertex[2].z, /**/ transform.color.r, transform.color.g, transform.color.b, /**/ mirrorX * frameWidth * currentFrame, mirrorY * 0.0f,
			vertex[3].x, vertex[3].y, vertex[3].z, /**/ transform.color.r, transform.color.g, transform.color.b, /**/ mirrorX * frameWidth * currentFrame, mirrorY * 1.0f
		};
		unsigned int indices[] = {
			0, 1, 3,  // First Triangle
			1, 2, 3   // Second Triangle
		};
		glm::mat4 modelMatrix = glm::mat4(1.0f);
		modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(transform.position.x, transform.position.y, transform.position.z));

		if (transform.scale.x != 0 || transform.scale.y != 0 || transform.scale.z != 0)
			modelMatrix = glm::scale(modelMatrix, glm::vec3(transform.scale.x, transform.scale.y, 0.0f));

		if (transform.rotationQuat.x != 0 || transform.rotationQuat.y != 0 || transform.rotationQuat.z != 0) {
			modelMatrix = glm::rotate(modelMatrix, glm::radians(transform.rotationQuat.x), glm::vec3(1.0, 0.0f, 0.0f));
			modelMatrix = glm::rotate(modelMatrix, glm::radians(transform.rotationQuat.y), glm::vec3(0.0f, 1.0f, 0.0f));
			modelMatrix = glm::rotate(modelMatrix, glm::radians(transform.rotationQuat.z), glm::vec3(0.0f, 0.0f, 1.0f));
		}

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glEnable(GL_TEXTURE_2D);
		glActiveTexture(GL_TEXTURE0);

		Renderer::setModelMatrix(modelMatrix);
		Renderer::drawTexture(vertexData, indices, spriteSheetID);

		glDisable(GL_BLEND);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glDisable(GL_TEXTURE_2D);
	}

}