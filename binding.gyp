{
	"targets": [
		{
			"target_name": "addon",
			"sources": [
				"source/addon.cc"
			],
			"dependencies": [
				"<!(node -p \"require('node-addon-api').targets\"):node_addon_api_except"
			],
			"conditions": [
				["OS=='win'", {
					"sources": [
						"source/win/mouse_hook.cc",
						"source/win/mouse.cc"
					],
					"include_dirs": [
						"source/win"
					]
				}],
				["OS=='mac'", {
					"sources": [
						"source/mac/mouse.cc"
					],
					"include_dirs": [
						"source/mac"
					],
					"xcode_settings": {
						"GCC_ENABLE_CPP_EXCEPTIONS": "YES",
						"CLANG_CXX_LIBRARY": "libc++",
						"CLANG_CXX_LANGUAGE_STANDARD": "c++17",
						"MACOSX_DEPLOYMENT_TARGET": "10.15"
					},
					"link_settings": {
						"libraries": [
							"/System/Library/Frameworks/ApplicationServices.framework"
						]
					}
				}]
			]
		}
	]
}
