#ifndef QRhi_h__
#define QRhi_h__

#ifdef ENABLE_QRHI
#include "private/qrhi_p.h"
//#include "private/qrhivulkan_p.h"
#include <QWindow>
#define QENGINECORE_API 

class QENGINECORE_API QShaderDefinitions {
public:
	QShaderDefinitions& addDefinition(const QByteArray def);
	QShaderDefinitions& addDefinition(const QByteArray def, int value);
	operator QByteArray() { return mDefinitions; }
private:
	QByteArray mDefinitions;
};

class QENGINECORE_API QRhiSignal {
public:
	QRhiSignal() {};
	void request();
	bool ensure();
	bool peek();
private:
	bool bDirty = false;
};

struct QENGINECORE_API QRhiHelper {
	struct QENGINECORE_API InitParams {
		QRhi::Implementation backend = QRhi::Vulkan;
		QRhi::Flags rhiFlags = QRhi::Flag();
		QRhiSwapChain::Flags swapChainFlags = QRhiSwapChain::Flag::SurfaceHasNonPreMulAlpha;
		QRhi::BeginFrameFlags beginFrameFlags;
		QRhi::EndFrameFlags endFrameFlags;
		int sampleCount = 1;
		bool enableStat = false;
	};

	static QSharedPointer<QRhi> create(QRhi::Implementation inBackend = QRhi::Vulkan, QRhi::Flags inFlags = QRhi::Flag(), QWindow* inWindow = nullptr);

	static QShader newShaderFromQSBFile(const char* filename);

	//static QRhiBuffer* newVkBuffer(QRhi* rhi, QRhiBuffer::Type type, VkBufferUsageFlags flags, int size);

	//static void setShaderResources(QRhiResource* pipeline, QRhiCommandBuffer* cb, QRhiShaderResourceBindings* srb = nullptr, int dynamicOffsetCount = 0, const QRhiCommandBuffer::DynamicOffset* dynamicOffsets = nullptr);
};
#endif

#endif // QRhi_h__
