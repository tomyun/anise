class Anise < Formula
  desc "ANISE: A New Implemented Scripting Engine for ai5v"
  homepage "http://tomyun.pe.kr/projectanise/"
  url "http://kldp.net/download.php/1051/anise-beta10-20040307-linux.tar.gz"
  version "0.10.20040307"
  sha256 "f2f0566df2b347a5b06a0cd8cd91f80fcbe70c474de814446570eb765d75fb5f"

  depends_on "autoconf" => :build
  depends_on "automake" => :build
  depends_on "sdl"

  def install
    system "./autogen.sh"
    system "./configure", "--disable-dependency-tracking",
                          "--disable-silent-rules",
                          "--prefix=#{prefix}"
    system "make", "install"
  end

  test do
    system "anise"
  end
end
