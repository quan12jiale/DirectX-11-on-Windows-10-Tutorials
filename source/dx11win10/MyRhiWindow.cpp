#ifdef ENABLE_QRHI
#include "MyRhiWindow.h"
#include "applicationclass.h"

MyRhiWindow::MyRhiWindow(QRhiHelper::InitParams inInitParams)
	: QRhiWindow(inInitParams) 
{
	mSigInit.request();			//请求初始化
	mSigSubmit.request();		//请求提交资源
}

void MyRhiWindow::InitializeData()
{
	// Set the number of vertices in the vertex array.
	m_vertexCount = 4;

	// Set the number of indices in the index array.
	m_indexCount = 6;

	m_vertexData.resize(m_vertexCount);
	m_indexData.resize(m_indexCount);

	// Load the vertex array with data.
	m_vertexData[0].position = XMFLOAT4(-1.0f, -1.0f, 0.0f, 1.0f);  // Bottom left.
	m_vertexData[0].color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);

	m_vertexData[1].position = XMFLOAT4(-1.0f, 1.0f, 0.0f, 1.0f);  // Top middle.
	m_vertexData[1].color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);

	m_vertexData[2].position = XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f);  // Bottom right.
	m_vertexData[2].color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);

	m_vertexData[3].position = XMFLOAT4(1.0f, -1.0f, 0.0f, 1.0f);  // Bottom right.
	m_vertexData[3].color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);

	// Load the index array with data.
	m_indexData[0] = 0;  // Bottom left.
	m_indexData[1] = 1;  // Top middle.
	m_indexData[2] = 2;  // Bottom right.
	m_indexData[3] = 0;  // Bottom right.
	m_indexData[4] = 2;  // Bottom right.
	m_indexData[5] = 3;  // Bottom right.
}

void MyRhiWindow::InitializeMatrix()
{
	view.lookAt(QVector3D(0.0f, 0.0f, -5.0f), QVector3D(0.0f, 0.0f, 1.0f), QVector3D(0.0f, 1.0f, 0.0f));
	//projection.perspective(3.141592654f / 4.0f, 800 / 600.0f, SCREEN_NEAR, SCREEN_DEPTH);

	projection = mRhi->clipSpaceCorrMatrix();
	projection.perspective(45.0f, 800 / 600.0f, SCREEN_NEAR, SCREEN_DEPTH);// 0.01f, 1000.0f
	//projection.translate(0, 0, -4);
}

bool MyRhiWindow::InitializeBuffers()
{
	if (mSigInit.ensure()) {	//初始化资源
		InitializeData();
		InitializeMatrix();
		/*
		* newBuffer(QRhiBuffer::Type type, QRhiBuffer::UsageFlags usage, quint32 size)
		* quint32 size => D3D11_BUFFER_DESC::ByteWidth
		*/
		m_vertexBuffer.reset(mRhi->newBuffer(QRhiBuffer::Immutable, QRhiBuffer::VertexBuffer, sizeof(VertexType) * m_vertexCount));
		if (!m_vertexBuffer->create())
			return false;
		QRhiVertexInputLayout inputLayout;
		inputLayout.setBindings({
			QRhiVertexInputBinding(sizeof(VertexType))		//定义每个VertexBuffer，单组顶点数据的跨度，这里是 6 * sizeof(float)，可以当作是GPU会从Buffer0（0是Index）读取 6 * sizeof(float) 传给 Vertex Shader
			});

		inputLayout.setAttributes({
			QRhiVertexInputAttribute(0, 0, QRhiVertexInputAttribute::Float4, 0),					// 从每组顶点数据的位置 0 开始作为 Location0（Float2） 的起始地址
			QRhiVertexInputAttribute(0, 1, QRhiVertexInputAttribute::Float4, sizeof(XMFLOAT4)),	// 从每组顶点数据的位置 sizeof(float) * 2 开始作为 Location1（Float4） 的起始地址
			});

		mPipeline.reset(mRhi->newGraphicsPipeline());
		mPipeline->setVertexInputLayout(inputLayout);

		m_indexBuffer.reset(mRhi->newBuffer(QRhiBuffer::Immutable, QRhiBuffer::IndexBuffer, sizeof(quint32) * m_indexCount));
		if (!m_indexBuffer->create())
			return false;

		m_matrixBuffer.reset(mRhi->newBuffer(QRhiBuffer::Dynamic, QRhiBuffer::UniformBuffer, 64 * 3));
		if (!m_matrixBuffer->create())
			return false;

		mShaderBindings.reset(mRhi->newShaderResourceBindings());															//创建绑定
		// Uniform Buffer 属于 着色器资源（Shader Resource） ，要使用它，需要在流水线的 着色器资源绑定（即描述符集布局）中，添加一个绑定项：
		mShaderBindings->setBindings({
			QRhiShaderResourceBinding::uniformBuffer(0, QRhiShaderResourceBinding::StageFlag::VertexStage, m_matrixBuffer.get()),
			});
		mShaderBindings->create();
		mPipeline->setShaderResourceBindings(mShaderBindings.get());														//绑定到流水线
		mPipeline->setSampleCount(mSwapChain->sampleCount());
		mPipeline->setRenderPassDescriptor(mSwapChainPassDesc.get());

		QShader vs = QRhiHelper::newShaderFromHlslFile(QShader::VertexStage, "../Engine/color.vert");
		Q_ASSERT(vs.isValid());

		QShader fs = QRhiHelper::newShaderFromHlslFile(QShader::FragmentStage, "../Engine/color.frag");
		Q_ASSERT(fs.isValid());

		mPipeline->setShaderStages({
			QRhiShaderStage(QRhiShaderStage::Vertex, vs),
			QRhiShaderStage(QRhiShaderStage::Fragment, fs)
			});
		// qrhid3d11.cpp文件toD3DTopology函数将QRhiGraphicsPipeline::Topology枚举映射为D3D11_PRIMITIVE_TOPOLOGY枚举
		// D3D不支持QRhiGraphicsPipeline::Topology::TriangleFan枚举
		// test:1、修改 图元 拓扑，绘制点，线。
		mPipeline->setTopology(QRhiGraphicsPipeline::Topology::Triangles);
		if (!mPipeline->create())
			return false;
	}

	QRhiRenderTarget* renderTarget = mSwapChain->currentFrameRenderTarget();	//交互链中的当前渲染目标
	QRhiCommandBuffer* cmdBuffer = mSwapChain->currentFrameCommandBuffer();		//交互链中的当前指令缓冲
	QRhiResourceUpdateBatch* batch = mRhi->nextResourceUpdateBatch();		//申请资源操作合批入口
	if (mSigSubmit.ensure()) {
		//QRhiResourceUpdateBatch* batch = mRhi->nextResourceUpdateBatch();		//申请资源操作合批入口
		batch->uploadStaticBuffer(m_vertexBuffer.get(), m_vertexData.data());				//上传顶点数据
		batch->uploadStaticBuffer(m_indexBuffer.get(), m_indexData.data());
		//cmdBuffer->resourceUpdate(batch);
	}
	batch->updateDynamicBuffer(m_matrixBuffer.get(), 0, 64, world.constData());
	batch->updateDynamicBuffer(m_matrixBuffer.get(), 64, 64, view.constData());
	batch->updateDynamicBuffer(m_matrixBuffer.get(), 128, 64, projection.constData());

	const QColor clearColor = QColor::fromRgbF(0.0f, 0.0f, 0.0f, 1.0f);			//使用该值来清理 渲染目标 中的 颜色附件
	const QRhiDepthStencilClearValue dsClearValue = { 1.0f,0 };					//使用该值来清理 渲染目标 中的 深度和模板附件
	/*
	nextResourceUpdateBatch() 是从池中获取可操作的 QRhiResourceUpdateBatch 实例，而当前可操作实例仅有一个，
	这意味着：在调用nextResourceUpdateBatch之后，再调用一次nextResourceUpdateBatch 就会出错。
	除非我们使用 QRhiCommandBuffer::resourceUpdate ， beginPass 或者 endPass，这些函数会处理 QRhiResourceUpdateBatch 实例，
	并destory它，让池中的下一个 QRhiResourceUpdateBatch 实例可以被正常使用。
	*/
	cmdBuffer->beginPass(renderTarget, clearColor, dsClearValue, batch);		//开启渲染通道 Note如果每次tick调用nextResourceUpdateBatch，需要把返回值传入beginPass

	cmdBuffer->setGraphicsPipeline(mPipeline.get());							//设置图形渲染管线
	cmdBuffer->setViewport(QRhiViewport(0, 0, mSwapChain->currentPixelSize().width(), mSwapChain->currentPixelSize().height()));		//设置图像的绘制区域
	cmdBuffer->setShaderResources();											//设置描述符集布局绑定，如果不填参数（为nullptr），则会使用渲染管线创建时所使用的描述符集布局绑定
	const QRhiCommandBuffer::VertexInput vertexInput(m_vertexBuffer.get(), 0);	//将 m_vertexBuffer 绑定到Buffer0，内存偏移值为0
	cmdBuffer->setVertexInput(0, 1, &vertexInput, m_indexBuffer.get(),0, QRhiCommandBuffer::IndexUInt32);
	//cmdBuffer->draw(m_vertexCount);															//执行绘制，其中 3 代表着有 3个顶点数据 输入
	cmdBuffer->drawIndexed(m_indexCount);//使用drawIndexed而不是draw，这样渲染会从IndexBuffer中，读取6个索引，利用顶点缓冲区中数据组装出6个顶点，来绘制三角形
	cmdBuffer->endPass();
	return true;
}

void MyRhiWindow::onRenderTick()
{
	InitializeBuffers();
}
#endif
