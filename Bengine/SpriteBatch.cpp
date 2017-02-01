#include "SpriteBatch.h"
#include <algorithm>
#include <iostream>

namespace Bengine {

SpriteBatch::SpriteBatch() : _vbo(0), _vao(0)
{
}


SpriteBatch::~SpriteBatch()
{
}


void SpriteBatch::init()
{
	createVertexArray();
}


void SpriteBatch::begin(GlyphSortType sortType /* GlyphSortType::TEXTURE */)
{
	_sortType = sortType;
	_renderBatches.clear();

	_glyphs.clear();
}


void SpriteBatch::end()
{
	// Set up all pointers for fast sorting
	_glyphPointers.resize(_glyphs.size());
	for (unsigned i = 0; i < _glyphs.size(); i++) {
		_glyphPointers[i] = &_glyphs[i];
	}

	sortGlyphs();
	createRenderBatches();
}


void SpriteBatch::draw(const glm::vec4& destRect, const glm::vec4& uvRect, GLuint texture, float depth, const ColorRGBA8& color)
{
	// Add the new Glyph to the glyphs
	_glyphs.emplace_back(Glyph(destRect, uvRect, texture, depth, color));
}


void SpriteBatch::renderBatch()
{
	glBindVertexArray(_vao);

	for (auto i = 0; i < _renderBatches.size(); i++) {
		glBindTexture(GL_TEXTURE_2D, _renderBatches[i].texture);

		glDrawArrays(GL_TRIANGLES, _renderBatches[i].offset, _renderBatches[i].numVertices);
	}

	glBindVertexArray(0);
}


void SpriteBatch::createRenderBatches()
{
	std::vector<Vertex> vertices;
	vertices.resize(_glyphPointers.size() * 6);

	if (_glyphPointers.empty()) return;

	int offset = 0;
	int cv = 0; // Current vertex
	_renderBatches.emplace_back(0, 6, _glyphPointers[0]->texture);

	for (int cg = 0; cg < _glyphPointers.size(); cg++) {
		if (cg > 0) {
			if (_glyphPointers[cg]->texture != _glyphPointers[cg - 1]->texture) {
				_renderBatches.emplace_back(offset, 6, _glyphPointers[cg]->texture);
			}
			else {
				_renderBatches.back().numVertices += 6;
			}
		}

		vertices[cv++] = _glyphPointers[cg]->topLeft;
		vertices[cv++] = _glyphPointers[cg]->bottomLeft;
		vertices[cv++] = _glyphPointers[cg]->bottomRight;

		vertices[cv++] = _glyphPointers[cg]->bottomRight;
		vertices[cv++] = _glyphPointers[cg]->topRight;
		vertices[cv++] = _glyphPointers[cg]->topLeft;

		offset += 6;
	}

	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	// Orphan the buffer
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);
	// Upload the data
	glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(Vertex), vertices.data());
	// Unbind the buffer
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}


void SpriteBatch::createVertexArray()
{
	if (_vao == 0) {
		glGenVertexArrays(1, &_vao);
	}

	// Bind the vertex array
	glBindVertexArray(_vao);

	if (_vbo == 0) {
		glGenBuffers(1, &_vbo);
	}

	// Bind the vertex buffer object
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);

	// Tell OpenGL that we want to use 3 attribute arrays (position, color, uv)
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	// This is the position attribute pointer
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, position));
	// This is the color attribute pointer
	glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), (void *)offsetof(Vertex, color));
	// This is the UV attribute pointer
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, uv));

	// Unbind the vertex array (disables all the stuff it's accosiated with like the position)
	glBindVertexArray(0);
}


void SpriteBatch::sortGlyphs()
{
	switch (_sortType) {
	case GlyphSortType::TEXTURE:
		std::stable_sort(_glyphPointers.begin(), _glyphPointers.end(), compareTexture);
		break;
	case GlyphSortType::FRONT_TO_BACK:
		std::stable_sort(_glyphPointers.begin(), _glyphPointers.end(), compareFrontToBack);
		break;
	case GlyphSortType::BACK_TO_FRONT:
		std::stable_sort(_glyphPointers.begin(), _glyphPointers.end(), compareBackToFront);
		break;
	}
}


bool SpriteBatch::compareFrontToBack(Glyph* a, Glyph* b)
{
	return (a->depth < b->depth);
}


bool SpriteBatch::compareBackToFront(Glyph* a, Glyph* b)
{
	return (a->depth > b->depth);
}


bool SpriteBatch::compareTexture(Glyph* a, Glyph* b)
{
	return (a->texture < b->texture);
}

}