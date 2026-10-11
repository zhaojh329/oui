<template>
  <el-card>
    <template #header>{{ $t('System') }}</template>
    <el-descriptions v-if="board && info" border :column="1">
      <el-descriptions-item :label="$t('Hostname')">{{ board.hostname }}</el-descriptions-item>
      <el-descriptions-item :label="$t('Operating system')">{{ board.release.description }}</el-descriptions-item>
      <el-descriptions-item :label="$t('Kernel')">{{ board.kernel }}</el-descriptions-item>
      <el-descriptions-item :label="$t('Uptime')">{{ uptime }}</el-descriptions-item>
      <el-descriptions-item :label="$t('Memory')">
        <el-progress :percentage="memoryUsage"/>
        {{ $t('Used') }}: {{ toMiB(info.memory.total - info.memory.available) }} MiB /
        {{ $t('Total') }}: {{ toMiB(info.memory.total) }} MiB
      </el-descriptions-item>
    </el-descriptions>
  </el-card>
</template>

<script>
export default {
  data() {
    return {
      board: null,
      info: null
    }
  },
  computed: {
    uptime() {
      const seconds = Math.floor(this.info.uptime)
      const days = Math.floor(seconds / 86400)
      const hours = Math.floor(seconds % 86400 / 3600)
      const minutes = Math.floor(seconds % 3600 / 60)

      return `${days} ${this.$t('days')} ${hours} ${this.$t('hours')} ${minutes} ${this.$t('minutes')}`
    },
    memoryUsage() {
      const { total, available } = this.info.memory

      return Number(((total - available) * 100 / total).toFixed(1))
    }
  },
  methods: {
    toMiB(bytes) {
      return (bytes / 1024 / 1024).toFixed(1)
    },
    async refresh() {
      try {
        const [board, info] = await Promise.all([
          this.$oui.call('system', 'board'),
          this.$oui.call('system', 'info')
        ])

        this.board = board
        this.info = info
      } catch {
        this.$message.error(this.$t('Unable to load system information'))
      }
    }
  },
  mounted() {
    this.$timer.create('system', () => this.refresh(), {
      time: 5000,
      repeat: true,
      immediate: true
    })
  }
}
</script>

<i18n src="./locale.json"/>
