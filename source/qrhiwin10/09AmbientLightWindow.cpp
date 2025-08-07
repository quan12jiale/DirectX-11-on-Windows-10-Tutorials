#include "09AmbientLightWindow.h"

bool AmbientLightWindow::InitializeAmbientLight()
{
	LoadTarga32Bit("../Engine/data/stone01.tga");
	mImage = QImage(m_targaData, m_width, m_height, QImage::Format_RGBA8888);
	Q_ASSERT(!mImage.isNull());

	mTexture.reset(mRhi->newTexture(QRhiTexture::RGBA8, mImage.size(), 1, QRhiTexture::MipMapped | QRhiTexture::UsedWithGenerateMips));
	if (!mTexture->create())
		return false;

	mSampler.reset(mRhi->newSampler(
		QRhiSampler::Filter::Linear,
		QRhiSampler::Filter::Linear,
		QRhiSampler::Filter::Linear,
		QRhiSampler::AddressMode::Repeat,
		QRhiSampler::AddressMode::Repeat,
		QRhiSampler::AddressMode::Repeat
	));
	mSampler->setTextureCompareOp(QRhiSampler::Always);
	if (!mSampler->create())
		return false;

	m_vertexBuffer.reset(mRhi->newBuffer(QRhiBuffer::Immutable, QRhiBuffer::VertexBuffer, sizeof(DiffuseLightVertexType) * m_vertexCount));
	if (!m_vertexBuffer->create())
		return false;

	m_indexBuffer.reset(mRhi->newBuffer(QRhiBuffer::Immutable, QRhiBuffer::IndexBuffer, sizeof(quint32) * m_indexCount));
	if (!m_indexBuffer->create())
		return false;

	m_matrixBuffer.reset(mRhi->newBuffer(QRhiBuffer::Dynamic, QRhiBuffer::UniformBuffer, 64 * 3));
	if (!m_matrixBuffer->create())
		return false;

	m_ambientLightBuffer.reset(mRhi->newBuffer(QRhiBuffer::Dynamic, QRhiBuffer::UniformBuffer, sizeof(AmbientLightBufferType)));
	if (!m_ambientLightBuffer->create())
		return false;

	mShaderBindings.reset(mRhi->newShaderResourceBindings());
	// Note: can't duplicates an existing binding number
	mShaderBindings->setBindings({
		QRhiShaderResourceBinding::uniformBuffer(0, QRhiShaderResourceBinding::StageFlag::VertexStage, m_matrixBuffer.get()),
		QRhiShaderResourceBinding::sampledTexture(1, QRhiShaderResourceBinding::StageFlag::FragmentStage, mTexture.get(), mSampler.get()),
		QRhiShaderResourceBinding::uniformBuffer(2, QRhiShaderResourceBinding::StageFlag::FragmentStage, m_ambientLightBuffer.get()),
		});
	if (!mShaderBindings->create())
		return false;

	mPipeline.reset(mRhi->newGraphicsPipeline());

	mPipeline->setSampleCount(mSwapChain->sampleCount());
	mPipeline->setTopology(QRhiGraphicsPipeline::Topology::Triangles);
	mPipeline->setPolygonMode(QRhiGraphicsPipeline::Fill);// Fill、Line
	mPipeline->setCullMode(QRhiGraphicsPipeline::Back);  // 启用背面剔除
	mPipeline->setFrontFace(QRhiGraphicsPipeline::CCW);   // 设置正面为逆时针

	QShader vs = QRhiHelper::newShaderFromHlslFile(QShader::VertexStage, "../Engine/light.vert");
	Q_ASSERT(vs.isValid());

	QShader fs = QRhiHelper::newShaderFromHlslFile(QShader::FragmentStage, "../Engine/light.frag");
	Q_ASSERT(fs.isValid());

	mPipeline->setShaderStages({
	QRhiShaderStage(QRhiShaderStage::Vertex, vs),
	QRhiShaderStage(QRhiShaderStage::Fragment, fs)
		});

	QRhiVertexInputLayout inputLayout;
	inputLayout.setBindings({
		QRhiVertexInputBinding(sizeof(DiffuseLightVertexType)) // 定义每个VertexBuffer，单组顶点数据的跨度，这里是 6 * sizeof(float)，可以当作是GPU会从Buffer0（0是Index）读取 6 * sizeof(float) 传给 Vertex Shader
		});
	const int position_offset = offsetof(DiffuseLightVertexType, position);
	const int texture_offset = offsetof(DiffuseLightVertexType, texture);
	const int normal_offset = offsetof(DiffuseLightVertexType, normal);
	inputLayout.setAttributes({
		QRhiVertexInputAttribute(0, 0, QRhiVertexInputAttribute::Float4, position_offset), // QRhiVertexInputAttribute::Float4, position_offset 从每组顶点数据的位置 0 开始作为 Location0（Float2） 的起始地址
		QRhiVertexInputAttribute(0, 1, QRhiVertexInputAttribute::Float2, texture_offset), // texture_offset 从每组顶点数据的位置 sizeof(float) * 2 开始作为 Location1（Float4） 的起始地址
		QRhiVertexInputAttribute(0, 2, QRhiVertexInputAttribute::Float3, normal_offset),
		});
	mPipeline->setVertexInputLayout(inputLayout);

	mPipeline->setShaderResourceBindings(mShaderBindings.get());
	mPipeline->setRenderPassDescriptor(mSwapChainPassDesc.get());
	if (!mPipeline->create())
		return false;
	return true;
}

void AmbientLightWindow::RenderAmbientLight()
{
	static float rotation = 360.0f;
	// Update the rotation variable each frame.
	rotation -= 0.174532925f * 1.1f;
	if (rotation <= 0.0f)
	{
		rotation += 360.0f;
	}
	QMatrix4x4 tmp_world = world;
	tmp_world.rotate(rotation, QVector3D(0, 1, 0));

	if (mSigInit.ensure()) { //初始化资源
		InitializeDiffuseLightVertexIndexData();
		InitializeMatrix();
		InitializeAmbientLight();
	}

	QRhiRenderTarget* renderTarget = mSwapChain->currentFrameRenderTarget(); //交互链中的当前渲染目标
	QRhiCommandBuffer* cmdBuffer = mSwapChain->currentFrameCommandBuffer(); //交互链中的当前指令缓冲
	QRhiResourceUpdateBatch* batch = mRhi->nextResourceUpdateBatch(); //申请资源操作合批入口
	if (mSigSubmit.ensure()) {
		batch->uploadStaticBuffer(m_vertexBuffer.get(), m_diffuseLightVertexData.data());
		batch->uploadStaticBuffer(m_indexBuffer.get(), m_indexData.data()); // m_indexData.data() IndexData
		batch->uploadTexture(mTexture.get(), mImage);
		batch->generateMips(mTexture.get());
		//cmdBuffer->resourceUpdate(batch);
	}
	batch->updateDynamicBuffer(m_matrixBuffer.get(), 0, 64, tmp_world.constData());
	batch->updateDynamicBuffer(m_matrixBuffer.get(), 64, 64, view.constData());
	batch->updateDynamicBuffer(m_matrixBuffer.get(), 128, 64, projection.constData());

	AmbientLightBufferType light_buf;
	// TODO 为什么 qrhi这样设置光照方向，效果是右边更亮
	light_buf.lightDirection = { 1.0f, 0.0f, 0.0f };
	light_buf.diffuseLightColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	light_buf.ambientLightColor = { 0.15f, 0.15f, 0.15f, 1.0f };
	batch->updateDynamicBuffer(m_ambientLightBuffer.get(), 0, sizeof(AmbientLightBufferType), &light_buf);

	const QColor clearColor = QColor::fromRgbF(0.0f, 0.0f, 0.0f, 1.0f); //使用该值来清理 渲染目标 中的 颜色附件
	const QRhiDepthStencilClearValue dsClearValue = { 1.0f, 0 }; //使用该值来清理 渲染目标 中的 深度和模板附件

	cmdBuffer->beginPass(renderTarget, clearColor, dsClearValue, batch); //开启渲染通道 Note如果每次tick调用nextResourceUpdateBatch，需要把返回值传入beginPass

	cmdBuffer->setGraphicsPipeline(mPipeline.get()); //设置图形渲染管线
	cmdBuffer->setViewport(QRhiViewport(0, 0, mSwapChain->currentPixelSize().width(), mSwapChain->currentPixelSize().height())); //设置图像的绘制区域
	cmdBuffer->setShaderResources(); //设置描述符集布局绑定，如果不填参数（为nullptr），则会使用渲染管线创建时所使用的描述符集布局绑定
	const QRhiCommandBuffer::VertexInput vertexInput(m_vertexBuffer.get(), 0); //将 m_vertexBuffer 绑定到Buffer0，内存偏移值为0
	cmdBuffer->setVertexInput(0, 1, &vertexInput, m_indexBuffer.get(), 0, QRhiCommandBuffer::IndexUInt32);
	cmdBuffer->drawIndexed(m_indexCount);// 使用drawIndexed而不是draw，这样渲染会从IndexBuffer中，读取6个索引，利用顶点缓冲区中数据组装出6个顶点，来绘制三角形
	cmdBuffer->endPass();
}

void AmbientLightWindow::onRenderTick()
{
	RenderAmbientLight();
}
