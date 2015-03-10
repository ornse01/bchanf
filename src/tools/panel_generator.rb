# -*- coding: euc-jp -*-
#
# panel_generator.rb
#
# Copyright (c) 2012 project bchan
#
# This software is provided 'as-is', without any express or implied
# warranty. In no event will the authors be held liable for any damages
# arising from the use of this software.
#
# Permission is granted to anyone to use this software for any purpose,
# including commercial applications, and to alter it and redistribute it
# freely, subject to the following restrictions:
#
# 1. The origin of this software must not be misrepresented; you must not
#    claim that you wrote the original software. If you use this software
#    in a product, an acknowledgment in the product documentation would be
#    appreciated but is not required.
#
# 2. Altered source versions must be plainly marked as such, and must not be
#    misrepresented as being the original software.
#
# 3. This notice may not be removed or altered from any source
#    distribution.
#

require 'yaml'
require 'erb'

def conv_euc_to_TCArray(str)
  ret = Array.new();
  str.encode("EUC-JP").each_char do |x|
    i = x.bytesize - 1;
    val = 0;
    x.each_byte do |ch|
      val += ch << (8 * i);
      i -= 1;
    end
    ret.push(val & 0x7f7f);
  end 
  ret;
end

def conv_TCArray_to_hex_definition(a)
  str = String.new();
  a.each do |x|
    str += "0x" + x.to_s(16) + ", "
  end
  str += "TNULL";
end

def calc_euc_to_TCArray_length(str)
  str = conv_euc_to_TCArray(str);
  return str.length;
end

class PanelItem
  attr_accessor :yaml
  def initialize(yaml)
    @yaml = yaml;
  end
  def type
    @yaml["type"]
  end
  def name
    @yaml["name"]
  end
  def item_text()
    @yaml["text"]
  end
  def width
    if @yaml["size"] != nil and @yaml["size"]["h"]
      return @yaml["size"]["h"]
    end
  end
  def height
    if @yaml["size"] != nil and @yaml["size"]["v"]
      return @yaml["size"]["v"]
    end
  end
  def item_text_to_hex_definition()
    conv_TCArray_to_hex_definition(conv_euc_to_TCArray(@yaml["text"]));
  end
  def item_text_length_in_TC
    return calc_euc_to_TCArray_length(@yaml["text"]);
  end

  def generate_pnl_item_value(i, left, top)
    return "";
  end
end

class PanelFixedTextItem < PanelItem
  def width
    w = super;
    if w != nil
      return w;
    end
    return 16 * item_text_length_in_TC;
  end
  def height
    h = super;
    if h != nil
      return h;
    end
    return 16;
  end

  def generate_pnl_item_value(i, left, top)
    script = <<-EOS
	pnl_item[<%= i %>].itype = TEXT_ITEM|ATR_TEXT;
	pnl_item[<%= i %>].info = 0;
	pnl_item[<%= i %>].ir = (RECT){{<%= left %>,<%= top %>,<%= left %>+<%= self.width %>,<%= top %>+<%= self.height %>}};
	pnl_item[<%= i %>].desc = 0;
	pnl_item[<%= i %>].dnum = 0;
	pnl_item[<%= i %>].ptr = (H*)(TC[]){<%= self.item_text_to_hex_definition() %>};
    EOS

    erb = ERB.new(script, nil, '-');
    erb.result(binding)
  end
end

class PanelNullItem < PanelItem
  def generate_pnl_item_value(i, left, top)
    script = <<-EOS
	pnl_item[<%= i %>].itype = NULL_ITEM;
	pnl_item[<%= i %>].info = 0;
	pnl_item[<%= i %>].ir = (RECT){{<%= left %>,<%= top %>,<%= left %>+<%= self.width %>,<%= top %>+<%= self.height %>}};
	pnl_item[<%= i %>].desc = 0;
	pnl_item[<%= i %>].dnum = 0;
	pnl_item[<%= i %>].ptr = NULL;
    EOS

    erb = ERB.new(script, nil, '-');
    erb.result(binding)
  end
end

class PanelButtonItem < PanelItem
  def width
    w = super;
    if w != nil
      return w;
    end
    return 16 * item_text_length_in_TC + 16;
  end
  def height
    h = super;
    if h != nil
      return h;
    end
    return 24;
  end

  def generate_pnl_item_value(i, left, top)
    script = <<-EOS
	pnl_item[<%= i %>].itype = PARTS_ITEM;
	pnl_item[<%= i %>].info = 0;
	pnl_item[<%= i %>].ir = (RECT){{<%= left %>,<%= top %>,<%= left %>+<%= self.width %>,<%= top %>+<%= self.height%>}};
	pnl_item[<%= i %>].desc = 0;
	pnl_item[<%= i %>].dnum = 0;
	pnl_item[<%= i %>].ptr = (H*)&(SWSEL){MS_PARTS|P_DISP, (RECT){{0, 0, <%= self.width %>, <%= self.height %>}}, 0, (TC[]){MC_STR, <%= self.item_text_to_hex_definition() %>}, {0, 0, -1, 0}};
    EOS

    erb = ERB.new(script, nil, '-');
    erb.result(binding)
  end
end

def generate_parts(type, a)
  case type
  when "fixedtext"
    return PanelFixedTextItem.new(a);
  when "null_item"
    return PanelNullItem.new(a);
  when "button"
    return PanelButtonItem.new(a);
  end
end

class PanelLine
  attr_accessor :yaml, :items
  def initialize(yaml)
    @yaml = yaml;
    @items = Array.new();
    if yaml["items"] != nil
      yaml["items"].each { |a|
        i = generate_parts(a["type"], a)
        @items.push(i);
      };
    end
  end
  def layout
    case @yaml["layout"]
    when "flush left", "flush right", "centering", "justification"
      return @yaml["layout"];
    else
      return "flush left"
    end
  end

  def height
    h = 0;
    @items.each { |item|
      if h < item.height
        h = item.height;
      end
    }
    return h;
  end
  def width(margin)
    w = 0;
    @items.each { |item|
      w += item.width + margin;
    }
    return w;
  end
end

class Panel
  attr_accessor :yaml, :lines, :margin_left, :margin_top, :margin_right, :margine_bottom, :panel_padding_left, :panel_padding_top, :panel_padding_right, :panel_padding_bottom
  def initialize(yaml)
    @yaml = yaml;
    @lines = Array.new();
    if yaml["lines"] != nil
      yaml["lines"].each { |a|
        l = PanelLine.new(a);
        @lines.push(l);
      };
    end
    @margin_left = 4;
    @margin_top = 4;
    @margin_right = 4;
    @margin_bottom = 4;
    @panel_padding_left = 20;
    @panel_padding_top = 20;
    @panel_padding_right = 20;
    @panel_padding_bottom = 20;
  end
  def panel_name()
    @yaml["panel_name"]
  end
  def panel_function_name()
    self.panel_name();
  end
  def panel_result_type_name()
    self.panel_name().upcase + "_RESULT";
  end
  def panel_arguments()
    ""
  end

  def each_item()
    @lines.each { |l|
      l.items.each { |i|
        yield(i);
      }
    }
  end
  def number_items
    n = 0;
    each_item() { |i|
      n += 1;
    }
    return n;
  end
  def each_item_type(type)
    @lines.each { |l|
      l.items.each { |i|
        if i.type == type
          yield(i);
        end
      }
    }
  end

  def content_width
    w = @margin_left + @margin_right;
    @lines.each { |line|
      w2 = @margin_left + line.width(@margin_left + @margin_right) + @margin_right;
      if w2 > w
        w = w2
      end
    }
    return w;
  end
  def content_height
    h = 0;
    @lines.each { |line|
      h += @margin_top + line.height + @margin_bottom;
    }
    return h;
  end

  def generate_header_retval_definition()
    script = <<-EOS
enum <%= panel_result_type_name() %>_ {
<%- self.each_item_type("button") do |i| -%>
	<%= panel_result_type_name().upcase %>_<%= i.name().upcase %>,
<%- end -%>
};
typedef enum <%= panel_result_type_name() %>_ <%= panel_result_type_name() %>;
    EOS

    erb = ERB.new(script, nil, '-');
    erb.result(binding)
  end
  def generate_header_function()
    script = <<-EOS
IMPORT <%= panel_result_type_name() %> <%= panel_function_name() %>(<%= panel_arguments() %>);
    EOS

    erb = ERB.new(script, nil, '-');
    erb.result(binding)
  end

  def calc_line_layout_left(content_width, line)
    case line.layout
    when "flush left", "justification"
      return @panel_padding_left + @margin_left;
    when "flush right"
      return @panel_padding_left + content_width - line.width(@margin_left + @margin_right);
    when "centering"
      return @panel_padding_left + (content_width - line.width(@margin_left + @margin_right)) / 2;
    end
  end
  def calc_line_layout_item_margin(content_width, line)
    case line.layout
    when "justification"
      return @margin_right + @margin_left + (content_width - line.width(@margin_left + @margin_right)) / (line.items.length - 1);
    else
      return @margin_right + @margin_left;
    end
  end

  def generate_source_function_pnl_item_value_each()
    cw = self.content_width;
    n = 0;
    top = @panel_padding_top + @margin_top;
    @lines.each { |l|
      left = self.calc_line_layout_left(cw, l);
      margin = self.calc_line_layout_item_margin(cw, l);
      l.items.each { |i|
        s = i.generate_pnl_item_value(n, left, top);
        yield(s);
        n += 1;
        left += i.width + margin;
      }
      top += l.height + @margin_bottom + @margin_top;
    }
  end
  def generate_source_function_ret_handler()
    n = 0;
    script = <<-EOS
<%- self.each_item() do |i| -%><%- if i.type == "button" -%>
			if (itemno == (<%= n %> + 1)) {
				ret = <%= panel_result_type_name().upcase %>_<%= i.name().upcase %>;
				break;
			}
<%- end -%><%- n += 1 -%><%- end -%>
    EOS

    erb = ERB.new(script, nil, '-');
    erb.result(binding)
  end

  def generate_source_function_function_name()
    script = <<-EOS
EXPORT <%= panel_result_type_name() %> <%= panel_function_name() %>(<%= panel_arguments() %>)
    EOS

    erb = ERB.new(script, nil, '-');
    erb.result(binding)
  end

  def generate_source_function_pnl_item_value()
    script = <<-EOS
<%- self.generate_source_function_pnl_item_value_each() do |s| -%><%= s %><%- end -%>
    EOS

    erb = ERB.new(script, nil, '-');
    erb.result(binding)
  end

  def generate_source_function()
    script = <<-EOS
<%= self.generate_source_function_function_name() %>{
	PNL_ITEM pnl_item[<%= number_items %>];
	PNID pnid0;
	PNT p0 = {0x8000,0x8000};
	WEVENT wev0;
	W stat,itemno;
	<%= panel_result_type_name().upcase %> ret;
	PANEL pnl = {
		2,0x48,0,
		{{0, 0, <%= self.panel_padding_left + self.content_width + self.panel_padding_right %>, <%= self.panel_padding_top + self.content_height + self.panel_padding_bottom %>}},
		0,
		<%= number_items %>,
		pnl_item
	};

<%= self.generate_source_function_pnl_item_value() %>
	pnid0 = pcre_pnl(&pnl, &p0);
	if (pnid0 < 0) {
		DP_ER("pcre_pnl error", pnid0);
		return pnid0;
	}

	for (;;) {
		ret = -1;
		stat = pact_pnl(pnid0, &wev0.e, &itemno);
		switch (stat) {
		case	P_EVENT:
			if (wev0.s.type == EV_DEVICE) {
				oprc_dev(&wev0.e, NULL, 0);
			}
			continue;
		default:
<%= self.generate_source_function_ret_handler() %>
			if (itemno >= 0) {
				continue;
			}
		case	0x5001:
<%= self.generate_source_function_ret_handler() %>
			if (itemno >= 0) {
				continue;
			}
			break;
		}
		if (ret != -1) {
			break;
		}
	}

	pdel_pnl(pnid0);

	return ret;
}

    EOS

    erb = ERB.new(script, nil, '-');
    erb.result(binding)
  end
end

class PanelData
  attr_accessor :yaml, :panels, :name
  def initialize(yaml)
    @yaml = yaml;
    @panels = Array.new();
    yaml["panels"].each { |a|
      p = Panel.new(a);
      @panels.push(p);
    };
  end
  def lisence_header()
    @yaml["generator"]["lisence_header"]
  end
  def lisence_source()
    @yaml["generator"]["lisence_source"]
  end
  def filename_header()
    @yaml["generator"]["output_header"]
  end

  def generate_header()
    script = <<-EOS
<%- @panels.each do |p| -%>
<%= p.generate_header_retval_definition() %>
<%= p.generate_header_function() %>
<%- end -%>
    EOS

    erb = ERB.new(script, nil, '-');
    erb.result(binding)
  end
  def generate_source_include_files()
    script = <<-EOS
#include    "<%= self.filename_header() %>"

#include	<bstdio.h>
#include	<bstdlib.h>
#include	<tcode.h>
#include	<tstring.h>
#include	<btron/btron.h>
#include	<btron/hmi.h>
#include	<btron/vobj.h>

#if PANEL_DEBUG
# define DP(arg) printf arg
# define DP_ER(msg, err) printf("%s (%d/%x)\\n", msg, err>>16, err)
#else
# define DP(arg) /**/
# define DP_ER(msg, err) /**/
#endif

    EOS

    erb = ERB.new(script, nil, '-');
    erb.result(binding)
  end
  def generate_source()
    script = <<-EOS
<%- @panels.each do |p| -%>
<%= p.generate_source_function() %>
<%- end -%>
    EOS

    erb = ERB.new(script, nil, '-');
    erb.result(binding)
  end
end

def generate_header(filename, data)
  fd = File.open(filename, "w");
  fd.puts data.lisence_header();
  fd.puts <<-EOS

/* This file is automatically generated. */

#include    <basic.h>
#include	<btron/dp.h>
#include	<btron/hmi.h>

  EOS
  fd.puts "#ifndef __" + filename.upcase.gsub(/\./, '_') + "__";
  fd.puts "#define __" + filename.upcase.gsub(/\./, '_') + "__";
  fd.puts "\n";
  fd.puts data.generate_header();
  fd.puts "#endif\n"
  fd.close
end

def generate_source(filename, data)
  fd = File.open(filename, "w");
  fd.puts data.lisence_source();
  fd.puts <<-EOS

/* This file is automatically generated. */

  EOS
  fd.puts data.generate_source_include_files();

  fd.puts data.generate_source();
  fd.close
end 

yaml = YAML.load(File.read(ARGV[0], encoding: 'EUC-JP:UTF-8'));
data = PanelData.new(yaml);

fname_header = yaml["generator"]["output_header"]
fname_source = yaml["generator"]["output_source"]

generate_header(fname_header, data);
generate_source(fname_source, data);
