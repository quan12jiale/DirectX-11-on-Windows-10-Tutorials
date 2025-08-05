#ifdef ENABLE_QRHI
#include "MyRhiWindow.h"
#include <QFile>
#include <QGuiApplication>

const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.3f;

MyRhiWindow::MyRhiWindow(QRhiHelper::InitParams inInitParams)
	: QRhiWindow(inInitParams) 
{
	mSigInit.request();			//请求初始化
	mSigSubmit.request();		//请求提交资源
}

MyRhiWindow::~MyRhiWindow()
{
	// Release the targa data.
	if (m_targaData)
	{
		delete[] m_targaData;
		m_targaData = 0;
	}
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
		D3D11_BUFFER_DESC desc = {};
		desc.ByteWidth = m_size;
		desc.Usage = m_type == Dynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
		desc.BindFlags = toD3DBufferUsage(m_usage);
		desc.CPUAccessFlags = m_type == Dynamic ? D3D11_CPU_ACCESS_WRITE : 0;
		desc.MiscFlags = m_usage.testFlag(QRhiBuffer::StorageBuffer) ? D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS : 0;
		ID3D11Device::CreateBuffer(const D3D11_BUFFER_DESC *pDesc, ...)
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

void MyRhiWindow::InitializeTextureVertexIndexData()
{
	// Set the number of vertices in the vertex array.
	m_vertexCount = 3;

	// Set the number of indices in the index array.
	m_indexCount = 3;

	// Create the vertex array.
	m_textureVertexData.resize(m_vertexCount);

	// Create the index array.
	m_indexData.resize(m_indexCount);

	// Load the vertex array with data.
	m_textureVertexData[0].position = XMFLOAT4(-1.0f, -1.0f, 0.0f, 1.0f);  // Bottom left.
	m_textureVertexData[0].texture = XMFLOAT2(0.0f, 1.0f);

	m_textureVertexData[1].position = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);  // Top middle.
	m_textureVertexData[1].texture = XMFLOAT2(0.5f, 0.0f);

	m_textureVertexData[2].position = XMFLOAT4(1.0f, -1.0f, 0.0f, 1.0f);  // Bottom right.
	m_textureVertexData[2].texture = XMFLOAT2(1.0f, 1.0f);

	// Load the index array with data.
	m_indexData[0] = 0;  // Bottom left.
	m_indexData[1] = 1;  // Top middle.
	m_indexData[2] = 2;  // Bottom right.
}

bool MyRhiWindow::InitializeTexture()
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

	m_vertexBuffer.reset(mRhi->newBuffer(QRhiBuffer::Immutable, QRhiBuffer::VertexBuffer, sizeof(TextureVertexType) * m_vertexCount));
	if (!m_vertexBuffer->create())
		return false;

	m_indexBuffer.reset(mRhi->newBuffer(QRhiBuffer::Immutable, QRhiBuffer::IndexBuffer, sizeof(quint32) * m_indexCount));
	if (!m_indexBuffer->create())
		return false;

	m_matrixBuffer.reset(mRhi->newBuffer(QRhiBuffer::Dynamic, QRhiBuffer::UniformBuffer, 64 * 3));
	if (!m_matrixBuffer->create())
		return false;

	mShaderBindings.reset(mRhi->newShaderResourceBindings());
	// Note: can't duplicates an existing binding number
	mShaderBindings->setBindings({
		QRhiShaderResourceBinding::uniformBuffer(0, QRhiShaderResourceBinding::StageFlag::VertexStage, m_matrixBuffer.get()),
		QRhiShaderResourceBinding::sampledTexture(1, QRhiShaderResourceBinding::StageFlag::FragmentStage, mTexture.get(), mSampler.get()),
		});
	if (!mShaderBindings->create())
		return false;

	mPipeline.reset(mRhi->newGraphicsPipeline());

	QRhiGraphicsPipeline::TargetBlend targetBlend;
	targetBlend.enable = false;
	//mPipeline->setTargetBlends({ targetBlend });

	mPipeline->setSampleCount(mSwapChain->sampleCount());
	mPipeline->setTopology(QRhiGraphicsPipeline::Topology::Triangles);
	mPipeline->setPolygonMode(QRhiGraphicsPipeline::Fill);// Fill、Line
	mPipeline->setCullMode(QRhiGraphicsPipeline::Back);  // 启用背面剔除
	mPipeline->setFrontFace(QRhiGraphicsPipeline::CCW);   // 设置正面为逆时针

	QShader vs = QRhiHelper::newShaderFromHlslFile(QShader::VertexStage, "../Engine/texture.vert");
	Q_ASSERT(vs.isValid());

	QShader fs = QRhiHelper::newShaderFromHlslFile(QShader::FragmentStage, "../Engine/texture.frag");
	Q_ASSERT(fs.isValid());

	mPipeline->setShaderStages({
	QRhiShaderStage(QRhiShaderStage::Vertex, vs),
	QRhiShaderStage(QRhiShaderStage::Fragment, fs)
		});

	QRhiVertexInputLayout inputLayout;
	inputLayout.setBindings({
		QRhiVertexInputBinding(sizeof(TextureVertexType)) // 定义每个VertexBuffer，单组顶点数据的跨度，这里是 6 * sizeof(float)，可以当作是GPU会从Buffer0（0是Index）读取 6 * sizeof(float) 传给 Vertex Shader
		});
	const int position_offset = offsetof(TextureVertexType, position);
	const int texture_offset = offsetof(TextureVertexType, texture);
	inputLayout.setAttributes({
		QRhiVertexInputAttribute(0, 0, QRhiVertexInputAttribute::Float4, position_offset), // QRhiVertexInputAttribute::Float4, position_offset 从每组顶点数据的位置 0 开始作为 Location0（Float2） 的起始地址
		QRhiVertexInputAttribute(0, 1, QRhiVertexInputAttribute::Float2, texture_offset), // texture_offset 从每组顶点数据的位置 sizeof(float) * 2 开始作为 Location1（Float4） 的起始地址
		});
	mPipeline->setVertexInputLayout(inputLayout);

	mPipeline->setShaderResourceBindings(mShaderBindings.get());
	mPipeline->setRenderPassDescriptor(mSwapChainPassDesc.get());
	if (!mPipeline->create())
		return false;
	return true;
}

void MyRhiWindow::RenderTexture()
{
	if (mSigInit.ensure()) { //初始化资源
		InitializeTextureVertexIndexData();
		InitializeMatrix();
		InitializeTexture();
	}

	QRhiRenderTarget* renderTarget = mSwapChain->currentFrameRenderTarget(); //交互链中的当前渲染目标
	QRhiCommandBuffer* cmdBuffer = mSwapChain->currentFrameCommandBuffer(); //交互链中的当前指令缓冲
	QRhiResourceUpdateBatch* batch = mRhi->nextResourceUpdateBatch(); //申请资源操作合批入口
	if (mSigSubmit.ensure()) {
		batch->uploadStaticBuffer(m_vertexBuffer.get(), m_textureVertexData.data()); // m_textureVertexData.data() VertexData
		batch->uploadStaticBuffer(m_indexBuffer.get(), m_indexData.data()); // m_indexData.data() IndexData
		batch->uploadTexture(mTexture.get(), mImage);
		batch->generateMips(mTexture.get());
		//cmdBuffer->resourceUpdate(batch);
	}
	batch->updateDynamicBuffer(m_matrixBuffer.get(), 0, 64, world.constData());
	batch->updateDynamicBuffer(m_matrixBuffer.get(), 64, 64, view.constData());
	batch->updateDynamicBuffer(m_matrixBuffer.get(), 128, 64, projection.constData());

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

bool MyRhiWindow::LoadTarga32Bit(const char* filename)
{
	// Open the targa file for reading in binary.
	QFile file(filename);
	if (!file.open(QIODevice::ReadOnly)) {
		return false;
	}

	QDataStream in(&file);    // read the data serialized from the file
	in.setByteOrder(QDataStream::LittleEndian); // 设置字节序 TGA文件格式指定使用小端字节序

	// Read in the file header.
	TargaHeader targaFileHeader;
	in >> targaFileHeader;

	// Get the important information from the header.
	m_height = (int)targaFileHeader.height;
	m_width = (int)targaFileHeader.width;
	int bpp = (int)targaFileHeader.bpp;

	// Check that it is 32 bit and not 24 bit.
	if (bpp != 32)
	{
		return false;
	}

	// Calculate the size of the 32 bit image data.
	int imageSize = m_width * m_height * 4;

	// Allocate memory for the targa image data.
	unsigned char* targaImage = new unsigned char[imageSize];
	// Read in the targa image data.
	for (int i = 0; i < imageSize; ++i) {
		in >> targaImage[i];
	}

	// Close the file.
	file.close();

	// Allocate memory for the targa destination data.
	m_targaData = new unsigned char[imageSize];

	// Initialize the index into the targa destination data array.
	int index = 0;

	// Initialize the index into the targa image data.
	int k = (m_width * m_height * 4) - (m_width * 4);

	// Now copy the targa image data into the targa destination array in the correct order since the targa format is stored upside down and also is not in RGBA order.
	for (int j = 0; j < m_height; j++)
	{
		for (int i = 0; i < m_width; i++)
		{
			m_targaData[index + 0] = targaImage[k + 2];  // Red.
			m_targaData[index + 1] = targaImage[k + 1];  // Green.
			m_targaData[index + 2] = targaImage[k + 0];  // Blue
			m_targaData[index + 3] = targaImage[k + 3];  // Alpha

			// Increment the indexes into the targa data.
			k += 4;
			index += 4;
		}

		// Set the targa image data index back to the preceding row at the beginning of the column since its reading it in upside down.
		k -= (m_width * 8);
	}

	// Release the targa image data now that it was copied into the destination array.
	delete[] targaImage;
	targaImage = 0;

	return true;
}

void MyRhiWindow::onRenderTick()
{
	//InitializeBuffers();
	RenderTexture();
}

QDataStream& operator<<(QDataStream& out, const MyRhiWindow::TargaHeader& header)
{
	// 写入 data1
	for (int i = 0; i < 12; ++i) {
		out << header.data1[i];
	}
	// 写入 width, height, bpp, data2
	out << header.width;
	out << header.height;
	out << header.bpp;
	out << header.data2;
	return out;
}

QDataStream& operator>>(QDataStream& in, MyRhiWindow::TargaHeader& header)
{
	// 读取 data1
	for (int i = 0; i < 12; ++i) {
		in >> header.data1[i];
	}
	// 读取 width, height, bpp, data2
	in >> header.width;
	in >> header.height;
	in >> header.bpp;
	in >> header.data2;
	return in;
}
#endif
