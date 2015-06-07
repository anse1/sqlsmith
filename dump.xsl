<?xml version='1.0'?>
<!-- translates sqlsmith graphml output into graphviz -->
<xsl:transform xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
	       xmlns:g="http://graphml.graphdrawing.org/xmlns"
	       xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
	       xsi:schemaLocation="http://graphml.graphdrawing.org/xmlns
				   http://graphml.graphdrawing.org/xmlns/1.0/graphml.xsd"
	       version='1.0'>

  <xsl:output method="text" />

  <xsl:template match="/g:graphml/g:graph">
    <xsl:text>digraph ast {</xsl:text>
    <xsl:apply-templates select="@*|node()"/>
    <xsl:text>}</xsl:text>
  </xsl:template>

  <xsl:template match="g:data">
    <xsl:choose>
      <xsl:when test="@key = 'label'">
	<xsl:text>label=</xsl:text>
	<xsl:value-of select="text()"/>
	<xsl:text>,</xsl:text>
      </xsl:when>
      <xsl:when test="@key = 'retries'">
	<xsl:choose>
	  <xsl:when test="text() > 1000">
	    <xsl:text>fillcolor=firebrick,style=filled</xsl:text>
	  </xsl:when>
	  <xsl:when test="text() > 500">
	    <xsl:text>fillcolor=red,style=filled</xsl:text>
	  </xsl:when>
	  <xsl:when test="text() > 100">
	    <xsl:text>fillcolor=salmon,style=filled</xsl:text>
	  </xsl:when>
	  <xsl:when test="text() > 50">
	    <xsl:text>fillcolor=wheat,style=filled</xsl:text>
	  </xsl:when>
	  <xsl:when test="text() > 10">
	    <xsl:text>fillcolor=springgreen,style=filled</xsl:text>
	  </xsl:when>
	  <xsl:when test="text() > 1">
	    <xsl:text>fillcolor=gainsboro,style=filled</xsl:text>
	  </xsl:when>
	  <xsl:otherwise>
	    <xsl:text>fillcolor=white,style=filled</xsl:text>
	  </xsl:otherwise>
	</xsl:choose>
	<xsl:text>,</xsl:text>
      </xsl:when>
      <xsl:otherwise>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <xsl:template match="g:node">
    <xsl:value-of select="@id"/>
    <xsl:text>[</xsl:text>
    <xsl:apply-templates select="g:data"/>
    <xsl:text>];
    </xsl:text>
  </xsl:template>

  <xsl:template match="g:edge">

    <xsl:value-of select="@source"/>
    <xsl:text> -> </xsl:text>
    <xsl:value-of select="@target"/>
    <xsl:text>;
    </xsl:text>
  </xsl:template>

</xsl:transform>
