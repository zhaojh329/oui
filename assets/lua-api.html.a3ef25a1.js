import{_ as n,o as s,c as a,e}from"./app.9ff75c61.js";const c={},t=e(`<h1 id="lua-\u63A5\u53E3" tabindex="-1"><a class="header-anchor" href="#lua-\u63A5\u53E3" aria-hidden="true">#</a> Lua \u63A5\u53E3</h1><p>\u5728 Oui \u4E2D\uFF0CLua \u63A5\u53E3\u4EE5 <code>\u6A21\u5757-\u65B9\u6CD5</code> \u7684\u5F62\u5F0F\u8FDB\u884C\u7EC4\u7EC7\u3002</p><div class="language-bash ext-sh line-numbers-mode"><pre class="language-bash"><code>root@OpenWrt:~<span class="token comment"># ls /usr/share/oui/rpc/</span>
acl.lua       network.lua   ubus.lua      ui.lua        wireless.lua
demo.lua      system.lua    uci.lua       user.lua
</code></pre><div class="line-numbers" aria-hidden="true"><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div></div></div><p>\u8FD9\u91CC\u7684\u6BCF\u4E2A Lua \u6587\u4EF6\u4EE3\u8868\u7740\u4E00\u4E2A\u6A21\u5757\u3002\u6A21\u5757\u540D\u4E3A Lua \u6587\u4EF6\u540D\uFF08\u4E0D\u5E26\u540E\u7F00\uFF09\u3002</p><p>\u6BCF\u4E2A Lua \u63A5\u53E3\u6587\u4EF6\u9700\u8981\u8FD4\u56DE\u4E00\u4E2A <code>Lua Table</code>\uFF0C\u8BE5 <code>Lua Table</code> \u7531\u591A\u4E2A <code>Lua function</code> \u7EC4\u6210\u3002</p><div class="language-lua ext-lua line-numbers-mode"><pre class="language-lua"><code><span class="token comment">-- /usr/share/oui/rpc/test.lua</span>

<span class="token keyword">local</span> M <span class="token operator">=</span> <span class="token punctuation">{</span><span class="token punctuation">}</span>

<span class="token comment">--[[
params: \u524D\u7AEF\u8C03\u7528\u4F20\u9012\u7684\u53C2\u6570
section: \u767B\u5F55\u7684\u4F1A\u8BDD\u4FE1\u606F\uFF0C\u4E3A\u4E00\u4E2A Table\uFF0C
         \u5305\u542B\u5F53\u524D\u767B\u5F55\u7684\u7528\u6237\u540D(username)\u548C\u5176\u6240\u5C5E\u7684\u6743\u9650\u7EC4(acl)
--]]</span>
<span class="token keyword">function</span> M<span class="token punctuation">.</span><span class="token function">func1</span><span class="token punctuation">(</span>params<span class="token punctuation">,</span> section<span class="token punctuation">)</span>
    <span class="token keyword">local</span> res <span class="token operator">=</span> <span class="token punctuation">{</span><span class="token punctuation">}</span>
    <span class="token punctuation">...</span>
    <span class="token keyword">return</span> res
<span class="token keyword">end</span>

<span class="token keyword">return</span> M
</code></pre><div class="line-numbers" aria-hidden="true"><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div></div></div><div class="language-javascript ext-js line-numbers-mode"><pre class="language-javascript"><code><span class="token keyword">this</span><span class="token punctuation">.</span><span class="token function">$oui</span><span class="token punctuation">.</span><span class="token function">call</span><span class="token punctuation">(</span><span class="token string">&#39;test&#39;</span><span class="token punctuation">,</span> <span class="token string">&#39;func1&#39;</span><span class="token punctuation">,</span> <span class="token punctuation">{</span><span class="token literal-property property">a</span><span class="token operator">:</span> <span class="token number">1</span><span class="token punctuation">}</span><span class="token punctuation">)</span><span class="token punctuation">.</span><span class="token function">then</span><span class="token punctuation">(</span><span class="token parameter">res</span> <span class="token operator">=&gt;</span> <span class="token punctuation">{</span>
    <span class="token operator">...</span>
<span class="token punctuation">}</span><span class="token punctuation">)</span>
</code></pre><div class="line-numbers" aria-hidden="true"><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div></div></div>`,7),l=[t];function p(i,o){return s(),a("div",null,l)}const r=n(c,[["render",p],["__file","lua-api.html.vue"]]);export{r as default};
