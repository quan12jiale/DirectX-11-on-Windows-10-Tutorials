#ifdef ENABLE_QRHI
#include "qd3dclass.h"

QD3DClass::QD3DClass()
{
}

QD3DClass::~QD3DClass()
{
	Shutdown();
}

bool QD3DClass::Initialize(int screenWidth, int screenHeight, bool vsync, HWND hwnd, bool fullscreen, float screenDepth, float screenNear)
{
	QRhiHelper::InitParams initParams;
	initParams.backend = QRhi::D3D11;
	mInitParams = initParams;

	m_window = QWindow::fromWinId(reinterpret_cast<WId>(hwnd));
	if (!m_window)
		return false;

	mRhi = QRhiHelper::create(mInitParams.backend, mInitParams.rhiFlags, m_window);
	if (!mRhi)
		return false;

	mSwapChain.reset(mRhi->newSwapChain());
	mDSBuffer.reset(mRhi->newRenderBuffer(QRhiRenderBuffer::DepthStencil, QSize(), mInitParams.sampleCount, QRhiRenderBuffer::UsedWithSwapChainOnly));

	mSwapChain->setWindow(m_window);
	mSwapChain->setDepthStencil(mDSBuffer.get());
	mSwapChain->setSampleCount(mInitParams.sampleCount);
	mSwapChain->setFlags(mInitParams.swapChainFlags);
	mSwapChainPassDesc.reset(mSwapChain->newCompatibleRenderPassDescriptor());
	mSwapChain->setRenderPassDescriptor(mSwapChainPassDesc.get());
	mSwapChain->createOrResize();
	return true;
}

void QD3DClass::Shutdown()
{
	delete m_window;
	m_window = nullptr;
}

void QD3DClass::BeginScene(float red, float green, float blue, float alpha)
{
	QRhi::FrameOpResult r = mRhi->beginFrame(mSwapChain.get(), mInitParams.beginFrameFlags);
	if (r == QRhi::FrameOpSwapChainOutOfDate) {
		r = mRhi->beginFrame(mSwapChain.get());
	}
	onRenderTick(red, green, blue, alpha);
}

void QD3DClass::EndScene()
{
	QRhi::FrameOpResult r = mRhi->endFrame(mSwapChain.get(), mInitParams.endFrameFlags);
	if (r == QRhi::FrameOpSwapChainOutOfDate) {
	}
}

void QD3DClass::onRenderTick(float red, float green, float blue, float alpha)
{
	QRhiRenderTarget* currentRenderTarget = mSwapChain->currentFrameRenderTarget();
	QRhiCommandBuffer* cmdBuffer = mSwapChain->currentFrameCommandBuffer();

	const QColor clearColor = QColor::fromRgbF(red, green, blue, alpha);
	const QRhiDepthStencilClearValue dsClearValue = { 1.0f,0 };

	cmdBuffer->beginPass(currentRenderTarget, clearColor, dsClearValue);
	cmdBuffer->endPass();
}
#endif