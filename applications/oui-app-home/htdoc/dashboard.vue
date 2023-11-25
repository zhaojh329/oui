<template>
  <el-space direction="vertical">
    <el-tooltip>
      <template #content>
        <slot></slot>
      </template>
      <el-progress type="dashboard" :stroke-width="10" :percentage="percentage" :color="color"/>
    </el-tooltip>
    <div>{{ label }}</div>
  </el-space>
</template>

<script>
export default {
  props: {
    label: String,
    percentage: Number,
    color: String
  }
}
</script>
