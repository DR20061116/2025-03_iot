<template>
	<view class="wrap">
		<view class="dev-area">

			<view class="dev-cart">
				<view>
					<view class="dev-name">温度</view>
					<view class="dev-icon">🌡️</view>
				</view>
				<view class="dev-data">{{t1}} ℃</view>
			</view>

			<view class="dev-cart">
				<view>
					<view class="dev-name">湿度</view>
					<view class="dev-icon">💧</view>
				</view>
				<view class="dev-data">{{h1}} %RH</view>
			</view>

			<view class="dev-cart">
				<view>
					<view class="dev-name">酒精浓度</view>
					<view class="dev-icon">🍶</view>
				</view>
				<view class="dev-data">{{c1}} %</view>
			</view>

			<view class="dev-cart">
				<view>
					<view class="dev-name">火焰探测</view>
					<view class="dev-icon">🔥</view>
				</view>
				<view class="dev-data">{{l1}} %</view>
			</view>

			<view class="dev-cart">
				<view>
					<view class="dev-name">可燃气体</view>
					<view class="dev-icon">⛽</view>
				</view>
				<view class="dev-data">{{o1}} %</view>
			</view>

			<view class="dev-cart">
				<view>
					<view class="dev-name">CO2浓度</view>
					<view class="dev-icon">🌫️</view>
				</view>
				<view class="dev-data">{{p1}} ppm</view>
			</view>

			<view class="dev-cart">
				<view>
					<view class="dev-name">喷淋</view>
					<view class="dev-icon">💦</view>
				</view>
				<switch :checked="spray" @change="onSpraySwitch" color="#2b9939" />
			</view>

			<view class="device-cart-l">
				<view>
					<view class="dev-name">温度上限</view>
					<view class="dev-name">{{t2}}℃</view>
				</view>
				<view class="ctrl-slider">
					<slider :value="t2" @change="sliderChange($event, 't2')" min="0" max="100" step="1" block-size="20" show-value />
				</view>
			</view>
			<view class="device-cart-l">
				<view>
					<view class="dev-name">温度下限</view>
					<view class="dev-name">{{t3}}℃</view>
				</view>
				<view class="ctrl-slider">
					<slider :value="t3" @change="sliderChange($event, 't3')" min="0" max="100" step="1" block-size="20" show-value />
				</view>
			</view>
			<view class="device-cart-l">
				<view>
					<view class="dev-name">湿度上限</view>
					<view class="dev-name">{{h2}}%RH</view>
				</view>
				<view class="ctrl-slider">
					<slider :value="h2" @change="sliderChange($event, 'h2')" min="0" max="100" step="1" block-size="20" show-value />
				</view>
			</view>
			<view class="device-cart-l">
				<view>
					<view class="dev-name">湿度下限</view>
					<view class="dev-name">{{h3}}%RH</view>
				</view>
				<view class="ctrl-slider">
					<slider :value="h3" @change="sliderChange($event, 'h3')" min="0" max="100" step="1" block-size="20" show-value />
				</view>
			</view>
			<view class="device-cart-l">
				<view>
					<view class="dev-name">酒精上限</view>
					<view class="dev-name">{{c2}}%</view>
				</view>
				<view class="ctrl-slider">
					<slider :value="c2" @change="sliderChange($event, 'c2')" min="0" max="100" step="1" block-size="20" show-value />
				</view>
			</view>
			<view class="device-cart-l">
				<view>
					<view class="dev-name">酒精下限</view>
					<view class="dev-name">{{c3}}%</view>
				</view>
				<view class="ctrl-slider">
					<slider :value="c3" @change="sliderChange($event, 'c3')" min="0" max="100" step="1" block-size="20" show-value />
				</view>
			</view>
			<view class="device-cart-l">
				<view>
					<view class="dev-name">火焰上限</view>
					<view class="dev-name">{{l2}}%</view>
				</view>
				<view class="ctrl-slider">
					<slider :value="l2" @change="sliderChange($event, 'l2')" min="0" max="100" step="1" block-size="20" show-value />
				</view>
			</view>
			<view class="device-cart-l">
				<view>
					<view class="dev-name">火焰下限</view>
					<view class="dev-name">{{l3}}%</view>
				</view>
				<view class="ctrl-slider">
					<slider :value="l3" @change="sliderChange($event, 'l3')" min="0" max="100" step="1" block-size="20" show-value />
				</view>
			</view>
			<view class="device-cart-l">
				<view>
					<view class="dev-name">可燃气体上限</view>
					<view class="dev-name">{{o2}}%</view>
				</view>
				<view class="ctrl-slider">
					<slider :value="o2" @change="sliderChange($event, 'o2')" min="0" max="100" step="1" block-size="20" show-value />
				</view>
			</view>
			<view class="device-cart-l">
				<view>
					<view class="dev-name">可燃气体下限</view>
					<view class="dev-name">{{o3}}%</view>
				</view>
				<view class="ctrl-slider">
					<slider :value="o3" @change="sliderChange($event, 'o3')" min="0" max="100" step="1" block-size="20" show-value />
				</view>
			</view>
			<view class="device-cart-l">
				<view>
					<view class="dev-name">CO2上限</view>
					<view class="dev-name">{{p2}}ppm</view>
				</view>
				<view class="ctrl-slider">
					<slider :value="p2" @change="sliderChange($event, 'p2')" min="0" max="2000" step="1" block-size="20" show-value />
				</view>
			</view>
			<view class="device-cart-l">
				<view>
					<view class="dev-name">CO2下限</view>
					<view class="dev-name">{{p3}}ppm</view>
				</view>
				<view class="ctrl-slider">
					<slider :value="p3" @change="sliderChange($event, 'p3')" min="0" max="2000" step="1" block-size="20" show-value />
				</view>
			</view>

		</view>
	</view>
</template>

<script>
const { createCommonToken } = require('@/key.js')

const product_id = 'Bb3Bt7Z0j0';
const device_name = 'd1';
const THRESHOLD_FIELDS = ['t2','t3','h2','h3','c2','c3','l2','l3','o2','o3','p2','p3'];
// 【新增】最大失败等待次数（3个同步周期，3*3=9秒）
const MAX_FAIL_ATTEMPTS = 3;

export default {
	data() {
		return {
			t1: '--', h1: '--', c1: '--', l1: '--', o1: '--', p1: '--',
			t2: 30, h2: 80, c2: 50, l2: 80, o2: 80, p2: 800,
			t3: 10, h3: 20, c3: 10, l3: 10, o3: 10, p3: 200,
			token: '',
			spray: false,
			thresholdTarget: {},
			thresholdLock: {},
			settingStatus: {},
			// 【新增】记录每个字段已经尝试同步了多少次
			settingAttemptCount: {}, 
			retryTimerMap: {},
			dataTimer: null,
		}
	},

	onLoad() {
		this.initToken();
		THRESHOLD_FIELDS.forEach(field => {
			this.thresholdTarget[field] = this[field];
			this.settingStatus[field] = 'idle';
			this.settingAttemptCount[field] = 0;
		});
		this.settingStatus.led = 'idle';
		this.settingAttemptCount.led = 0;
	},

	onShow() {
		this.startDataUpdate();
	},
	onHide() {
		this.stopDataUpdate();
		this.clearAllTimers();
	},
	onUnload() {
		this.stopDataUpdate();
		this.clearAllTimers();
	},

	methods: {
		initToken() {
			try {
				const params = {
					author_key: 'c0a787ec7ab54a4b87c594006cc06dba',
					version: '2022-05-01',
					user_id: '490050',
				}
				this.token = createCommonToken(params);
			} catch (e) {
				console.error('Token生成失败', e);
			}
		},

		clearAllTimers() {
			Object.values(this.retryTimerMap).forEach(id => clearTimeout(id));
			this.retryTimerMap = {};
		},
		clearFieldTimers(field) {
			if (this.retryTimerMap[field]) {
				clearTimeout(this.retryTimerMap[field]);
				delete this.retryTimerMap[field];
			}
		},

		// 【修改】同步间隔改为 3000ms (3秒)
		startDataUpdate() {
			this.stopDataUpdate(); 
			this.fetchDevData();
			this.dataTimer = setInterval(() => {
				this.fetchDevData();
			}, 3000);
		},
		
		stopDataUpdate() {
			if (this.dataTimer) {
				clearInterval(this.dataTimer);
				this.dataTimer = null;
			}
		},

		// 【核心逻辑】数据获取与判定
		fetchDevData() {
			if (!this.token) {
				this.initToken();
			}

			uni.request({
				url: 'https://iot-api.heclouds.com/thingmodel/query-device-property',
				method: 'GET',
				data: { product_id, device_name },
				header: { authorization: this.token },
				success: (res) => {
					if (res.data.code === 10403) {
						this.initToken(); 
						return;
					}
					if (!res.data.data) return;

					res.data.data.forEach(item => {
						const id = item.identifier;
						const val = item.value;

						// 阈值字段处理
						if (THRESHOLD_FIELDS.includes(id)) {
							// 正常数据更新（非设置中）
							if (this.settingStatus[id] !== 'setting' || !this.thresholdLock[id] || Date.now() >= this.thresholdLock[id]) {
								this[id] = val;
								this.thresholdTarget[id] = val;
							}
							
							// 【核心判定逻辑】
							if (this.settingStatus[id] === 'setting') {
								if (this.thresholdTarget[id] !== undefined && val === this.thresholdTarget[id]) {
									// 数据一致 -> 成功
									this.handleSetSuccess(id);
								} else {
									// 数据不一致 -> 增加计数
									this.settingAttemptCount[id]++;
									// 如果超过最大尝试次数 -> 失败
									if (this.settingAttemptCount[id] >= MAX_FAIL_ATTEMPTS) {
										this.handleSetFail(id);
									}
								}
							}
						}
						// 喷淋状态处理
						else if (id === 'led') {
							if (this.settingStatus.led !== 'setting' || !this.thresholdLock.led || Date.now() >= this.thresholdLock.led) {
								this.spray = val;
								this.thresholdTarget.led = val;
							}
							
							// 【核心判定逻辑】
							if (this.settingStatus.led === 'setting') {
								if (this.thresholdTarget.led !== undefined && val === this.thresholdTarget.led) {
									this.handleSetSuccess('led');
								} else {
									this.settingAttemptCount.led++;
									if (this.settingAttemptCount.led >= MAX_FAIL_ATTEMPTS) {
										this.handleSetFail('led');
									}
								}
							}
						}
						// 传感器数据强制更新
						else {
							this[id] = val;
						}
					});

					this.clearSyncedThresholds();
				},
				fail: (err) => {
					console.log('获取数据失败', err);
					// 【注意】这里可以选择是否在网络失败时也增加计数，目前暂不增加，只在有数据返回但不对时才判定
				}
			});
		},

		handleSetSuccess(field) {
			if (this.settingStatus[field] !== 'setting') return;
			this.settingStatus[field] = 'success';
			this.settingAttemptCount[field] = 0; // 重置计数
			this.clearFieldTimers(field);
			const fieldName = this.getFieldName(field);
			uni.showToast({ title: `${fieldName}设置成功`, icon: 'success', duration: 1500 });
			setTimeout(() => { this.settingStatus[field] = 'idle'; }, 1500);
		},

		handleSetFail(field) {
			if (this.settingStatus[field] !== 'setting') return;
			this.settingStatus[field] = 'fail';
			this.settingAttemptCount[field] = 0; // 重置计数
			this.clearFieldTimers(field);
			const fieldName = this.getFieldName(field);
			uni.showToast({ title: `${fieldName}设置失败，请重试`, icon: 'none', duration: 2000 });
			setTimeout(() => { this.settingStatus[field] = 'idle'; }, 2000);
		},

		getFieldName(field) {
			const nameMap = {
				't2': '温度上限', 't3': '温度下限',
				'h2': '湿度上限', 'h3': '湿度下限',
				'c2': '酒精上限', 'c3': '酒精下限',
				'l2': '火焰上限', 'l3': '火焰下限',
				'o2': '可燃气体上限', 'o3': '可燃气体下限',
				'p2': 'CO2上限', 'p3': 'CO2下限',
				'led': '喷淋'
			};
			return nameMap[field] || field;
		},

		clearSyncedThresholds() {
			THRESHOLD_FIELDS.forEach(field => {
				if (this.settingStatus[field] !== 'setting' && 
					this.thresholdTarget[field] !== undefined && 
					this[field] === this.thresholdTarget[field]) {
					delete this.thresholdTarget[field];
					delete this.thresholdLock[field];
				}
			});
			if (this.settingStatus.led !== 'setting' && 
				this.thresholdTarget.led !== undefined && 
				this.spray === this.thresholdTarget.led) {
				delete this.thresholdTarget.led;
				delete this.thresholdLock.led;
			}
		},

		sendWithRetry(field, value, retryCount = 0, maxRetries = 4) {
			if (this.settingStatus[field] !== 'setting') return;
			const params = { [field]: value };
			
			uni.request({
				url: 'https://iot-api.heclouds.com/thingmodel/set-device-property',
				method: 'POST',
				data: { product_id, device_name, params },
				header: { authorization: this.token, 'Content-Type': 'application/json' },
				success: () => { console.log(`发送成功 ${field}=${value}`); },
				fail: (err) => { console.log(`发送失败 ${field}=${value}`, err); }
			});

			if (retryCount < maxRetries) {
				this.retryTimerMap[field] = setTimeout(() => {
					this.sendWithRetry(field, value, retryCount + 1, maxRetries);
				}, 1000);
			}
		},

		// 【修改】滑块调节
		sliderChange(e, id) {
			const targetVal = e.detail.value;
			if (this.settingStatus[id] === 'setting') { 
				this.clearFieldTimers(id); 
			}
			
			this[id] = targetVal;
			this.thresholdTarget[id] = targetVal;
			this.thresholdLock[id] = Date.now() + 5000;
			
			// 【重置状态】
			this.settingStatus[id] = 'setting';
			this.settingAttemptCount[id] = 0; // 【新增】重置尝试次数
			
			const fieldName = this.getFieldName(id);
			uni.showLoading({ title: `${fieldName}设置中...`, mask: false });
			setTimeout(() => uni.hideLoading(), 1000);
			
			this.sendWithRetry(id, targetVal, 0);
			
			// 【移除】不再有 setTimeout 判定超时
		},

		// 【修改】喷淋开关
		onSpraySwitch(e) {
			const targetSpray = e.detail.value;
			const targetT2 = targetSpray ? 0 : 100;

			if (this.settingStatus.led === 'setting') this.clearFieldTimers('led');
			if (this.settingStatus.t2 === 'setting') this.clearFieldTimers('t2');

			this.spray = targetSpray;
			this.t2 = targetT2;
			this.thresholdTarget.led = targetSpray;
			this.thresholdTarget.t2 = targetT2;
			this.thresholdLock.led = Date.now() + 5000;
			this.thresholdLock.t2 = Date.now() + 5000;

			// 【重置状态】
			this.settingStatus.led = 'setting';
			this.settingStatus.t2 = 'setting';
			this.settingAttemptCount.led = 0;
			this.settingAttemptCount.t2 = 0;
			
			uni.showLoading({ title: '喷淋设置中...', mask: false });
			setTimeout(() => uni.hideLoading(), 1000);

			this.sendWithRetry('led', targetSpray, 0);
			this.sendWithRetry('t2', targetT2, 0);
			
			// 【移除】不再有 setTimeout 判定超时
		},
	}
}
</script>

<style>
	.wrap { padding: 30rpx; }
	.dev-area { display: flex; justify-content: space-between; flex-wrap: wrap; }
	.dev-cart { height: 150rpx; width: 320rpx; border-radius: 30rpx; margin-top: 30rpx; display: flex; justify-content: space-around; align-items: center; box-shadow: 0 0 15rpx #ccc; }
	.device-cart-l { height: 150rpx; width: 700rpx; border-radius: 30rpx; margin-top: 30rpx; display: flex; justify-content: space-around; align-items: center; box-shadow: 0 0 15rpx #ccc; }
	.dev-name { font-size: 20rpx; text-align: center; color: #6d6d6d; }
	.dev-icon { font-size: 70rpx; margin-top: 10rpx; text-align: center; }
	.dev-data { font-size: 50rpx; color: #6d6d6d; }
	.ctrl-slider { width: 580rpx; }
</style>