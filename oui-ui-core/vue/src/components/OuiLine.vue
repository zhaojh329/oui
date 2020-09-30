<template>
  <div :style="{backgroundColor: color, height: width + 'px', width: length}" class="oui-line">
    <img v-if="disconnect" src="/icons/error.png" :style="{marginTop: (width / 2 - 20) + 'px'}" />
    <div v-else-if="active" class="oui-line-active" :style="{height: width + 'px'}"/>
  </div>
</template>

<script>
export default {
  props: {
    color: {
      type: String,
      default: '#56E67E'
    },
    width: {
      type: Number,
      default: 8
    },
    length: {
      type: String,
      default: '100%'
    },
    disconnect: Boolean,
    active: Boolean
  }
}
</script>

<style lang="stylus">
.oui-line {
  text-align: center;
  margin: auto 10px auto;

  .oui-line-active {
    position: relative;
    width: 10px;
    background: #09f;
    border-radius: 10px;
    animation: oui-line-active 4s ease-in-out infinite;
  }
  @keyframes oui-line-active {
    0% {
      left: 0;
    }
    50% {
      left: calc(100% - 10px);
    }
    100% {
      left: 0;
    }
  }
}
</style>
